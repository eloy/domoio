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
  device.assert_data_eq("406 Not Acceptable\n");
  domoio::stop_server();
}


TEST(CreateSession, valid_device) {
  const char *password = "0123456789abcdef";
  domoio::NetworkDevice *m_device = domoio::factory_network_device(1, "foo", password);


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


TEST(CreateSession, from_device) {
  const char *session = "5b 62 7a a0 cf 09 b5 ae 40 27 a7 5b 65 7b 10 60 1410013485";
  const char *received = "2a 83 7e 67 c8 95 05 6a 47 3d 26 c1 8c 51 ea 01 4b 51 95 37 ea 68 95 d3 eb f9 90 2f 2d 47 b2 5d 00 ";
  int bytes_transfered = 99;
  const char *password = "0123456789abcdef";

  int length = (AES_IV_LENGTH * 3);

  unsigned char *iv = domoio::crypto::hex_decode(session, &length);
  domoio::crypto::BlockCipher bc(password, iv);

  int received_length = bytes_transfered + -1;
  unsigned char *crypted = domoio::crypto::hex_decode(received, &received_length);

  received_length = 33;
  char *clean = bc.decrypt(crypted, &received_length);

  free(crypted);
  free(clean);
}
