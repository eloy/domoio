#include "domoio.h"

namespace domoio {
  namespace commands {

    static void listen_events(ControlConnection *conn, CommandParams params) {
      LOG(trace) << "Start listening events";
      conn->start_listening_events();
      conn->send("200 OK");
    }

    bool start_events_registered = domoio::register_control_command("start_events", new ControlCommandDef(&listen_events, 0, "Start receiving events"));
  }
}
