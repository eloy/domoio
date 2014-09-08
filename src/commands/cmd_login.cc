#include "domoio.h"

namespace domoio {
  namespace commands {

    static void login(Connection *conn, CommandParams params) {
      if (conn->login(params->at(1).c_str())) {
        conn->send("200 WELCOME");
      } else {
        conn->send("BAD LOGIN");
        conn->close();
      }
    }

    bool cmd_login_registered = domoio::register_server_command("login", new CommandDef(&login, 1, "Sign in"));

  }
}
