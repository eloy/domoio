#include "domoio_test.h"


namespace domoio {

  bool TestControl::connect(void) {
    try {
      boost::asio::local::stream_protocol::endpoint ep(conf_opt::socket_path);
      this->socket.connect(ep);
      this->read();
      EXPECT_STREQ(&this->buffer[0], "Hey, protocol=1.0\n");
    }
    catch (std::exception& e) {
      LOG(fatal) << "TestControl connection error: " << e.what();
    }

    return true;
  }

  bool TestControl::close(void) {
    socket.close();
    return true;
  }


  bool TestControl::read(void) {
    try {
      memset(buffer, 0, CLIENT_BUFFER_MAX_LENGTH);
      boost::system::error_code error;

      size_t bytes_transferred = socket.read_some(boost::asio::buffer(buffer, CLIENT_BUFFER_MAX_LENGTH), error);

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

  bool TestControl::send(std::string msg) {
    LOG(trace) << "Sending: " << msg;
    boost::asio::write(socket, boost::asio::buffer(msg.c_str(), msg.length()));
    return true;
  }

  char *TestControl::data(void) {
    return &this->buffer[0];
  }

}
