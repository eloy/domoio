#include "domoio_test.h"

TEST(cmd_all_ports_state, set_state) {
  const char *password = "0123456789abcdef";
  domoio::Device *m_device = domoio::factory_device(1, "foo", password);

  domoio::start_server();
  boost::asio::io_service io_service;

  domoio::TestDevice device(io_service, 1, password);

  device.connect();
  device.login();

  device.send("aps 0:0, 1:1, 2:0, 3:1");
  device.read();

  ASSERT_EQ(m_device->port(0)->value(), 0);
  ASSERT_EQ(m_device->port(1)->value(), 1);
  ASSERT_EQ(m_device->port(2)->value(), 0);
  ASSERT_EQ(m_device->port(3)->value(), 1);
  device.close();
  domoio::stop_server();
}
