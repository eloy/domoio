#include "domoio.h"

namespace domoio {
  namespace commands {

    void create_session(DeviceConnection *conn, CommandParams params) {
      int device_id = atoi(params->at(1).c_str());
      if (!conn->create_session(device_id)) {
        conn->close();
      }
    }

  }
}
