#include "domoio_test.h"

TEST(Signals, send_event) {
  const char *password = "0123456789abcdef";
  domoio::NetworkDevice *m_device = domoio::factory_network_device(1, "foo", password);


  domoio::start_server();
  boost::asio::io_service io_service;

  domoio::TestDevice device(io_service, 1, password);
  device.connect();
  device.login();
  m_device->network_signals("foo bar wadus");
  device.read();
  device.assert_data_eq("foo bar wadus");
  device.close();
  domoio::stop_server();
}
