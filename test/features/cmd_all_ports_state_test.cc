#include "models.h"
#include "domoio_test.h"

TEST(cmd_all_ports_state, set_state) {
  const char *password = "0123456789abcdef";

  domoio::Device m_device;
  domoio::load_device(&m_device, "foo", password);
  m_device.save();

  domoio::start_server();

  boost::asio::io_service io_service;

  domoio::TestDevice device(io_service, m_device.get_id(), password);

  device.connect();
  device.login();

  domoio::DeviceState *state = domoio::DeviceState::find(m_device.get_id());
  EXPECT_TRUE(state != NULL);

  device.send("aps 0:0, 1:1, 2:0, 3:1");
  device.read();

  ASSERT_EQ(state->port(0)->value(), 0);
  ASSERT_EQ(state->port(1)->value(), 1);
  ASSERT_EQ(state->port(2)->value(), 0);
  ASSERT_EQ(state->port(3)->value(), 1);

  device.close();
  domoio::stop_server();
}
