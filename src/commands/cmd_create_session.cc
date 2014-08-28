#include "domoio.h"

namespace domoio {
  namespace commands {

    static void create_session(DeviceConnection *conn, CommandParams params) {
      int device_id = atoi(params->at(1).c_str());
      if (!conn->create_session(device_id)) {
        conn->close();
      }
    }

    int cmd_create_session_id = domoio::register_server_command("create_session", &create_session);

  }
}
