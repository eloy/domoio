#include "domoio.h"

namespace domoio {
  namespace commands {
    void get_devices(ControlConnection *conn, CommandParams params) {
      conn->send(devices_to_json());
    }
  }
}
