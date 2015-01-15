#include "log.h"
#include "helpers.h"
#include "httpd.h"
#include "models.h"
#include "devices.h"
#include <boost/filesystem/path.hpp>

#define DEFAULT_EXTENSION ".html"

namespace domoio {
  namespace httpd {
    namespace actions {


      const std::string load_asset(std::string filename) {
        std::string www_root = "/Users/harlock/src/c/domoio/web/public";
        boost::filesystem::path path(www_root);
        path += filename;

        // If filename has no extension, add the default
        if (path.extension().string().length() == 0) {
          path += DEFAULT_EXTENSION;
        }


        if (!helpers::file_exists(path.string())) {
          throw std::string("File not found: " +  path.string());
        }

        return helpers::read_file(path.string());
      }


      const std::string INDEX = load_asset("/index.html");

      bool static_content(Request *request) {
        LOG(error) << "Static Content: " << request->url;

        if (strcmp(request->url, "/") == 0) {
          request->response_data(INDEX);
          return true;
        }

        try {
          std::string content = load_asset(request->url);
          request->response_data(content);
          return true;
        }
        catch(std::string ex) {
          LOG(error) << ex;
        }

        return false;
      }

    }
  }
}
