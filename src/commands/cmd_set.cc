#include "domoio.h"

namespace domoio {
  namespace commands {

    void set(DeviceConnection *conn, CommandParams params) {
      std::string target_url = params->at(1);
      std::string state = params->at(2);
      device_set_port_state(target_url, atol(state.c_str()));
      conn->send("OK");
    }

  }
}
