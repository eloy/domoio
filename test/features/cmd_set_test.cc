#include "domoio_test.h"

TEST(cmd_set, send_signal_to_device) {
  const char *password = "0123456789abcdef";
  domoio::Device *m_device_1 = domoio::factory_device(1, "foo", password);
  domoio::Device *m_device_2 = domoio::factory_device(2, "foo", password);

  domoio::start_server();
  boost::asio::io_service io_service_1;
  boost::asio::io_service io_service_2;

  domoio::TestDevice device_1(io_service_1, 1, password);
  domoio::TestDevice device_2(io_service_2, 2, password);

  device_1.connect();
  device_1.login();

  device_2.connect();
  device_2.login();

  device_1.send("set 2::1 1");
  BOOST_LOG_TRIVIAL(trace) << "Reading";

  device_2.read();
  device_2.assert_data_eq("set 1 1");

  device_1.close();
  device_2.close();

  domoio::stop_server();
}
