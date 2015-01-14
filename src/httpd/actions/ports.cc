#include "models.h"
#include "devices.h"
#include "httpd.h"

namespace domoio {
  namespace httpd {
    namespace actions {


      bool set_port(Request *request) {
        int device_id = request->param_int("device_id");
        int port_id = request->param_int("id");

        if (!request->post_data_received) return false;

        DeviceState *device = DeviceState::find(device_id);
        if (device == NULL) return false;

        PortState *port = device->port(port_id);
        if (port == NULL) return false;

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
        int id = request->param_int("id");

        Device device;
        if (device.load_from_db(device_id) == false) {
          return false;
        }

        // SHOW
        if (request->is_get()) {
          Port *port = device.get_port(id);
          if (port == NULL) {
            return false;
          }

          request->response_data(port->to_json());
          return true;
        }


        // Create
        if (request->is_post()) {
          Port *port = new Port();
          port->from_json(request->post_data_raw.str());
          device.add_port(port);
          device.save();
          request->response_data(port->to_json());
          LOG(trace) << "Port created";
          return true;
        }

        // Update
        if (request->is_put()) {
          Port *port = device.get_port(id);
          if (port == NULL) {
            return false;
          }

          port->from_json(request->post_data_raw.str());
          device.save();
          request->response_data(port->to_json());
          LOG(trace) << "Port created";
          return true;
        }

        return false;
      }


    }
  }
}
