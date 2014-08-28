#include "domoio_test.h"

TEST(CreateSession, invalid_device) {
  domoio::start_server();
  boost::asio::io_service io_service;

  domoio::TestDevice device(io_service);
  device.connect();
  device.read();
  device.assert_data_eq("Hey, protocol=1.0\n");
  device.send("create_session 1");
  device.read();
  device.assert_data_eq("406 Not Acceptable");
  domoio::stop_server();
}


TEST(CreateSession, valid_device) {
  const char *password = "012345678";
  domoio::Device *m_device = domoio::factory_device(1, "foo", password);


  domoio::start_server();
  boost::asio::io_service io_service;

  domoio::TestDevice device(io_service, 1, password);
  device.connect();
  device.read();
  device.assert_data_eq("Hey, protocol=1.0\n");
  device.send("create_session 1");
  device.read();
  device.start_session(device.get_data());
  device.send("login 1234");
  device.read();
  device.assert_data_eq("200 WELCOME");
  device.close();
  domoio::stop_server();
}
