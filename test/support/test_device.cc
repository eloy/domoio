#include "domoio_test.h"


namespace domoio {

  bool TestDevice::connect(void) {
    try {
      boost::asio::ip::tcp::resolver resolver(io_service);
      boost::asio::ip::tcp::resolver::query query("127.0.0.1", "8013");
      boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
      boost::asio::connect(socket, endpoint_iterator);
    }
    catch (std::exception& e) {
      std::cerr << e.what() << std::endl;
    }

    return true;
  }

  bool TestDevice::close(void) {
    socket.close();
    return true;
  }

  bool TestDevice::read(void) {
    try {
      memset(data, 0, CLIENT_BUFFER_MAX_LENGTH);
      boost::system::error_code error;

      size_t len = socket.read_some(boost::asio::buffer(data, CLIENT_BUFFER_MAX_LENGTH), error);

      if (error == boost::asio::error::eof)
        return false;
      else if (error)
        throw boost::system::system_error(error); // Some other error.
    }
    catch (std::exception& e) {
      std::cerr << e.what() << std::endl;
      return false;
    }
    return true;
  }

  bool TestDevice::send(std::string msg) {
    if (this->session_started) {
      this->send_crypted(msg.c_str(), msg.length());
    } else {
      this->send_raw(msg.c_str(), msg.length());
    }
    return true;
  }

  bool TestDevice::send_raw(const char *str, int length) {
    boost::asio::write(socket, boost::asio::buffer(str, length + 1));
    return true;
  }

  bool TestDevice::send_crypted(const char *str, int length) {
    this->send_raw(str, length);
    return true;
  }


  char *TestDevice::get_data(void) {
    return &this->data[0];
  }

  bool TestDevice::start_session(const char *str) {
    this->block_cipher = new domoio::crypto::BlockCypher(this->password);
    // Override current IV with the one in str
    int iv_hex_length = AES_IV_LENGTH * 3;
    domoio::crypto::hex_decode(&this->block_cipher->salt[0], str, iv_hex_length);
    this->session_started = true;
    return true;
  }

  void TestDevice::assert_data_eq(const char* str) {
    ASSERT_STREQ(str, this->data);
  }

}
