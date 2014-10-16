#include "domoio_test.h"

TEST(cmd_set, send_signal_to_device) {
  const char *password = "0123456789abcdef";
  domoio::NetworkDevice *m_device = domoio::factory_network_device(1, "foo", password);
  domoio::start_server();
  boost::asio::io_service io_service_1;
  boost::asio::io_service io_service_2;

  domoio::TestDevice device_1(io_service_1, 1, password);
  domoio::TestControl control(io_service_2);

  device_1.connect();
  device_1.login();

  control.connect();
  control.send("set 1::1 1");
  control.read();

  device_1.read();
  ASSERT_STREQ(device_1.get_data(), "set 1 1");

  device_1.close();
  control.close();
  domoio::stop_server();
}
