#include "domoio_server.h"

namespace domoio {
  namespace commands {

    void all_ports_state(DeviceConnection *conn, CommandParams params) {

      int ports_count = params->size() - 1;
      DeviceState *device = conn->get_device();

      for (int i = 0; i < ports_count; i++) {
        int id, value;
        sscanf(params->at(i + 1).c_str(), "%d:%d", &id, &value);

        PortState *port = device->port(id);

        if (port == NULL) {
          LOG(error) << "Invalid port: " << id;
          conn->send("400 Bad Request");
          conn->close();
          return;
        }

        if (port->value() != value) {
          port->set_value(value, true);
          // TODO send change event
        }
      }

      conn->send("200 OK");
    }
  }
}
