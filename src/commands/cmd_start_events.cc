#include "domoio.h"

namespace domoio {
  namespace commands {

    void listen_events(ControlConnection *conn, CommandParams params) {
      LOG(trace) << "Start listening events";
      conn->start_listening_events();
      conn->send("200 OK");
    }
  }
}
