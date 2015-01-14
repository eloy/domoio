#include "domoio_server.h"

namespace domoio {
  namespace commands {
    void exit(DeviceConnection *conn, CommandParams params) {
      conn->close();
    }
  }
}
