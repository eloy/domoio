#include "domoio.h"

namespace domoio {
  namespace commands {

    static void login(DeviceConnection *conn, CommandParams params) {
      if (conn->login(params->at(1).c_str())) {
        conn->send("200 WELCOME");
      } else {
        conn->send("BAD LOGIN");
        conn->close();
      }
    }

    bool cmd_login_registered = domoio::register_device_command("login", new DeviceCommandDef(&login, 1, "Sign in"));

  }
}
