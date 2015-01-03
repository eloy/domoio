#include "domoio.h"
#include "httpd.h"

namespace domoio {
  namespace httpd {
    namespace actions {

      bool set_port(Request *request) {
        int device_id = request->param_int("device_id");
        int port_id = request->param_int("id");

        if (!request->post_data_received) return false;

        Device * device = device_find(device_id);
        if (!device) return false;
        Port * port = device->port(port_id);
        if (!port) return false;

        json::Object doc;
        json::Reader::Read(doc, request->post_data_raw);
        if (port->digital()) {
          json::Boolean &v = doc["value"];
          port->set_value(v.Value());
        } else {
          json::Number &v = doc["value"];
          int value = v.Value();
          port->set_value(value);
        }

        request->response_data("OK");
        return true;
      }



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
