#include "domoio_test.h"


namespace domoio {

  bool TestDevice::connect(void) {
    try {
      boost::asio::ip::tcp::resolver resolver(io_service);
      boost::asio::ip::tcp::resolver::query query("127.0.0.1", boost::to_string(conf_opt::port));
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

      size_t bytes_transferred = socket.read_some(boost::asio::buffer(data, CLIENT_BUFFER_MAX_LENGTH), error);

      if (this->session_started) {
        int len =  bytes_transferred;
        unsigned char *crypted = domoio::crypto::hex_decode(this->data, &len);
        char * clean = this->block_cipher->decrypt(crypted, &len);
        memset(data, 0, CLIENT_BUFFER_MAX_LENGTH);
        memcpy(data, clean, len);
        free(crypted);
        free(clean);
      }

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
    unsigned char * enc = this->block_cipher->encrypt(str, &length);
    char *hex = domoio::crypto::hex_encode(enc, &length);
    this->send_raw(hex, length);
    free(enc);
    free(hex);
    return true;
  }


  char *TestDevice::get_data(void) {
    return &this->data[0];
  }

  bool TestDevice::start_session(const char *str) {
    // Override current IV with the one in str
    int length = (AES_IV_LENGTH * 3) + 1;

    unsigned char *iv = domoio::crypto::hex_decode(str, &length);
    this->block_cipher = new domoio::crypto::BlockCipher(this->password, iv);
    this->session_started = true;
    free(iv);
    return true;
  }

  bool TestDevice::login() {
    this->read();
    this->assert_data_eq("Hey, protocol=1.0\n");
    this->send("create_session 1");
    this->read();
    this->start_session(this->get_data());
    this->send("login 1234");
    this->read();
    this->assert_data_eq("200 WELCOME");
    return true;
  }

  void TestDevice::assert_data_eq(const char* str) {
    ASSERT_STREQ(str, this->data);
  }

}
