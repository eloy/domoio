#include "domoio.h"

namespace domoio {
  namespace commands {
    static void exit(ControlConnection *conn, CommandParams params) {
      conn->close();
    }

    bool cmd_exit_registerd = domoio::register_control_command("exit", new ControlCommandDef(&exit, 0, "Exit"));
  }
}
