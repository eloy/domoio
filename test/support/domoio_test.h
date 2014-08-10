#ifndef DOMOIO_TEST_H
#define DOMOIO_TEST_H
#include <gtest/gtest.h>
#include "domoio.h"


#define DOMOIO_ENV_TEST

namespace domoio {


  class TestEnvironment : public ::testing::Environment {
  public:
    virtual ~TestEnvironment() {}

    virtual void SetUp() {
      domoio::db::connect();
    }


    virtual void TearDown() {
      domoio::db::close();
    }
  };



  class DomoioTest  : public testing::Test {
  protected:
    virtual void SetUp() {

    }
  };

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
