#include "domoio.h"

namespace domoio {
  namespace commands {
    void exit(ControlConnection *conn, CommandParams params) {
      conn->close();
    }
  }
}
