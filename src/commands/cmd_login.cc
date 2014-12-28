#include "domoio.h"

namespace domoio {
  namespace commands {

    void login(DeviceConnection *conn, CommandParams params) {
      if (conn->login(params->at(1).c_str())) {
        conn->send("200 WELCOME");
      } else {
        conn->send("BAD LOGIN");
        conn->close();
      }
    }
  }
}
