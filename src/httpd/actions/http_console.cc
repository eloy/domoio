#include "log.h"
#include "httpd.h"
#include "jsengine.h"
#include "uuid.h"

namespace domoio {
  namespace httpd {
    namespace actions {


      std::map<std::string, js::JsProcessor*> sessions;

      bool console(Request *request) {
        int id = request->param_int("id");

        // INDEX
        if (request->is_get() && id == 0) {
          request->response_data("{\"sessions\":[]}");
          return true;
        }


        // POST -> create new session
        if (request->is_post() && id == 0 ) {
          std::string uuid = generate_uuid();
          request->response_data("{\"id\":\"" + uuid + "\"}");
          return true;
        }


        request->response_data("{\"result\":\"foo\"}");
        return true;
      }
    }
  }
}
