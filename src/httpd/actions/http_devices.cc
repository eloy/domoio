#include "log.h"
#include "httpd.h"
#include "models.h"
#include "devices.h"

namespace domoio {
  namespace httpd {
    namespace actions {
      bool devices(Request *request) {
        int id = request->param_int("id");

        // INDEX
        if (request->is_get() && id == 0) {
          vault::ModelsCollection<Device> collection;
          Device::all(&collection);
          request->response_data(collection.to_json());
          return true;
        }

        // SHOW
        if (request->is_get() && id != 0) {
          Device device;
          device.load_from_db(id);
          request->response_data(device.to_json());
          return true;
        }

        // Create
        if (request->is_post() && id == 0) {
          Device device;
          device.from_json(request->post_data_raw());
          device.save();
          request->response_data(device.to_json());
          LOG(error) << "Device created";
          return true;
        }

        // Update
        if (request->is_put() && id != 0) {
          Device device;
          device.load_from_db(id);
          device.from_json(request->post_data_raw());
          device.save();
          request->response_data(device.to_json());
          LOG(error) << "Device Updated";
          return true;
        }

        return false;
      }
    }
  }
}
