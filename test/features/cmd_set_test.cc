#include "domoio_test.h"

// TEST(cmd_set, send_signal_to_device) {
//   const char *password = "0123456789abcdef";

//   domoio::Device m_device;
//   domoio::load_device(&m_device, "foo", password);
//   m_device.save();

//   domoio::start_server();
//   boost::asio::io_service io_service_1;
//   boost::asio::io_service io_service_2;

//   domoio::TestDevice device_1(io_service_1, m_device.get_id(), password);
//   domoio::TestControl control(io_service_2);

//   device_1.connect();
//   device_1.login();

//   domoio::DeviceState *state = domoio::DeviceState::find(m_device.get_id());
//   EXPECT_TRUE(state != NULL);


//   control.connect();
//   control.send("set 1::1 1");
//   control.read();

//   device_1.read();
//   ASSERT_STREQ(device_1.get_data(), "set 1 1");

//   device_1.close();
//   control.close();
//   domoio::stop_server();
// }
