#include "domoio_test.h"


namespace domoio {
  TestDevice::TestDevice(boost::asio::io_service& _io_service) :
    io_service(_io_service),  socket(_io_service) {
  }

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

  char *TestDevice::get_data(void) {
    return &this->data[0];
  }

  void TestDevice::assert_data_eq(const char* str) {
    EXPECT_EQ(strcmp(this->data, str), 0);
  }

}
