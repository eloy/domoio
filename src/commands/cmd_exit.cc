#include "domoio.h"

namespace domoio {
  namespace commands {
    void exit(DeviceConnection *conn, CommandParams params) {
      conn->close();
    }
  }
}
