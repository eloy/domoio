#include "domoio.h"
#include "httpd.h"

namespace domoio {
  namespace httpd {
    namespace actions {
      bool devices(Request *request) {
        int id = request->param_int("id");

        // INDEX
        if (id == 0) {
          std::string data = devices_to_json();
          request->response = MHD_create_response_from_buffer(data.length(), (void*)data.c_str(), MHD_RESPMEM_MUST_COPY);
          request->status = MHD_HTTP_OK;
          return true;
        }

        return false;
      }
    }
  }
}
