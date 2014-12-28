#include "domoio_test.h"

TEST(events, send_global_events) {
  domoio::events::start();
  domoio::events::send(new domoio::Event(domoio::events::port_set, domoio::events::private_channel));
  ASSERT_TRUE(domoio::expect_events(1));
  domoio::events::stop();
}


// TEST(events, control_receive_events) {
//   domoio::NetworkDevice *m_device = domoio::factory_network_device(1, "foo", "0123456789abcdef");
//   domoio::start_server();

//   boost::asio::io_service io_service_1;
//   boost::asio::io_service io_service_2;
//   boost::asio::io_service io_service_3;

//   domoio::TestDevice device_1(io_service_1, 1, "0123456789abcdef");
//   domoio::TestControl control_1(io_service_2);
//   domoio::TestControl control_2(io_service_3);

//   device_1.connect();
//   device_1.login();

//   control_1.connect();
//   control_1.send("start_events");
//   control_1.read();

//   control_2.connect();
//   control_2.send("set 1::1 1");
//   control_2.read();

//   control_1.read();

//   const char* expected = "event_data private {\n    \"type\": \"port_set\",\n    \"device_id\": \"1\",\n    \"port_id\": \"1\",\n    \"value\": \"1\",\n    \"old_value\": \"0\"\n}\n";
//   ASSERT_STREQ(control_1.data(), expected);

//   device_1.close();
//   control_1.close();
//   control_2.close();
//   domoio::stop_server();
// }
