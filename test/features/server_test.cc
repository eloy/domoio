#include "domoio_test.h"

TEST(Server, Listen_localhost) {
  domoio::start_server();
  boost::asio::io_service io_service;

  domoio::TestDevice device(io_service);
  device.connect();
  device.read();
  device.assert_data_eq("Hey, protocol=1.0\n");
  device.close();
  domoio::stop_server();
}
