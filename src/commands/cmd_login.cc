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

    int cmd_login_id = domoio::register_server_command("login", &login);

  }
}
