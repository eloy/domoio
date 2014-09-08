#include "domoio.h"

namespace domoio {
  namespace commands {

    static void create_session(Connection *conn, CommandParams params) {
      int device_id = atoi(params->at(1).c_str());
      if (!conn->create_session(device_id)) {
        conn->close();
      }
    }

    bool cmd_create_session_registerd = domoio::register_server_command("create_session", new CommandDef(&create_session, 1, "Start session"));

  }
}
