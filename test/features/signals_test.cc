#include "models.h"
#include "domoio_test.h"

TEST(Signals, send_event) {
  const char *password = "0123456789abcdef";

  domoio::Device *m_device = domoio::factory_device("foo", password, true);
  m_device->save();
  int id = m_device->get_id();
  delete(m_device);

  domoio::DeviceState::load_virtual_devices();

  domoio::start_server();
  boost::asio::io_service io_service;
  domoio::TestDevice device(io_service, id, password);

  device.connect();
  device.login();

  domoio::DeviceState *state = domoio::DeviceState::find(id);
  state->network_signals("foo bar wadus");
  device.read();
  device.assert_data_eq("foo bar wadus");
  device.close();
  domoio::stop_server();

}
