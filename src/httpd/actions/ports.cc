#include "domoio.h"
#include "httpd.h"

namespace domoio {
  namespace httpd {
    namespace actions {
      bool ports(Request *request) {
        int device_id = request->param_int("device_id");
        int port_id = request->param_int("id");

        // Update
        if (request->is_put()) {
          if (request->post_data_received) {
            LOG(error) << "Updating port: " << port_id << " with: " << request->post_data_raw;
          }

          request->response_data("OK");
          return true;
        }
        return false;
      }
    }
  }
}
