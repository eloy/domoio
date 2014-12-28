#include "domoio.h"

namespace domoio {
  namespace commands {

    void all_ports_state(DeviceConnection *conn, CommandParams params) {
      int ports_count = params->size() - 1;
      Device *device = conn->get_device();

      for (int i = 0; i < ports_count; i++) {
        int id, value;
        sscanf(params->at(i + 1).c_str(), "%d:%d", &id, &value);

        Port *port = device->port(id);
        if (port->value() != value) {
          port->set_value(value);
          // TODO send change event
        }
      }

      conn->send("200 OK");
    }
  }
}
