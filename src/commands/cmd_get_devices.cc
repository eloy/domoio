#include "domoio.h"

namespace domoio {
  namespace commands {
    static void get_devices(ControlConnection *conn, CommandParams params) {
      conn->send(devices_to_json());
    }

    bool cmd_get_devices_registerd = domoio::register_control_command("get_devices", new ControlCommandDef(&get_devices, 0, "Return Devices"));
  }
}
