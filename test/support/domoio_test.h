#ifndef DOMOIO_TEST_H
#define DOMOIO_TEST_H
#include <gtest/gtest.h>
#include "domoio.h"

namespace domoio {


  class TestDevice {
  public:
    TestDevice(boost::asio::io_service&);
    bool connect(void);
    bool read(void);
    char* get_data(void);
    void assert_data_eq(const char*);

  private:
    boost::asio::io_service& io_service;
    boost::asio::ip::tcp::socket socket;
    char data[CLIENT_BUFFER_MAX_LENGTH];
  };


}

#endif //DOMOIO_TEST_H
