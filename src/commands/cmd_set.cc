#include "domoio.h"

namespace domoio {
  namespace commands {

    static void set(Connection *conn, CommandParams params) {
      std::string target_url = params->at(1);
      std::string state = params->at(2);
      device_set_port_state(target_url, atol(state.c_str()));
      conn->send("OK");
    }

    bool cmd_set_registered = domoio::register_server_command("set", new CommandDef(&set, 2, "Set value at a port"));

  }
}
