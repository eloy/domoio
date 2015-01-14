#include "domoio.h"
#include "events.h"

namespace domoio {
  int events_counter;

  void callback(domoio::EventPtr event_ptr) {
    events_counter++;
  }

  bool expect_events(int expected) {
    domoio::events::add_listener(&callback);
    events_counter = 0;
    int i = 0;
    while(events_counter < expected || i < 1000) {
      i++;
    }
    return events_counter == expected;
  }
}
