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


TEST(Server, invalid_command) {
  domoio::start_server();
  boost::asio::io_service io_service;

  domoio::TestDevice device(io_service);
  device.connect();
  device.read();
  device.send("foo");
  device.read();
  device.assert_data_eq("400 Bad Request\n");
  device.close();
  domoio::stop_server();
}

TEST(Server, empty_command) {
  domoio::start_server();
  boost::asio::io_service io_service;

  domoio::TestDevice device(io_service);
  device.connect();
  device.read();
  device.send("");
  device.read();
  device.assert_data_eq("400 Bad Request\n");
  device.close();
  domoio::stop_server();
}


TEST(Server, cmd_exit) {
  domoio::start_server();
  boost::asio::io_service io_service;

  domoio::TestDevice device(io_service);
  device.connect();
  device.read();
  device.send("exit");
  domoio::stop_server();
}
