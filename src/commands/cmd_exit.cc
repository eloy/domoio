#include "domoio.h"

namespace domoio {
  namespace commands {
    static void exit(Connection *conn, CommandParams params) {
      conn->close();
    }

    bool cmd_exit_registerd = domoio::register_server_command("exit", new CommandDef(&exit, 0, "Exit"));
  }
}
