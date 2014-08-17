#include "domoio_test.h"

TEST(Login, unauthorized) {
  domoio::start_server();
  boost::asio::io_service io_service;

  domoio::TestDevice device(io_service);
  device.connect();
  device.read();
  device.assert_data_eq("Hey, protocol=1.0\n");
  device.send("foo");
  device.read();
  device.assert_data_eq("401 Unauthorized");
  device.close();
  domoio::stop_server();
}



TEST(Login, bad_login) {
  domoio::start_server();
  boost::asio::io_service io_service;

  domoio::TestDevice device(io_service);
  device.connect();
  device.read();
  device.assert_data_eq("Hey, protocol=1.0\n");
  device.send("login 1 bar");
  device.read();
  device.assert_data_eq("403 Forbidden");
  device.close();
  domoio::stop_server();
}
