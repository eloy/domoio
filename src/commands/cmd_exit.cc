#include "domoio.h"

namespace domoio {
  namespace commands {
    static void exit(DeviceConnection *conn, CommandParams params) {
      conn->close();
    }

    int cmd_exit_id = domoio::register_server_command("exit", &exit);
  }
}
