#include "domoio_test.h"


bool test_bool = false;

void callback(domoio::Event* event) {
  test_bool = true;
}

TEST(events, send_global_events) {
  domoio::events::start();
  domoio::events::send(new domoio::Event("test_event"));
  domoio::events::add_listener(&callback);
  int count = 0;

  while(!test_bool || count < 1000) {
    count++;
  }

  ASSERT_TRUE(test_bool);
  domoio::events::stop();
}
