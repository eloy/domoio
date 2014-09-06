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
      domoio::setup_config_options();
      domoio::init_domoio();
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
    TestDevice(boost::asio::io_service &_io_service) :
    io_service(_io_service),  socket(_io_service) {
      this->session_started = false;
    }

    TestDevice(boost::asio::io_service& _io_service, int _id, const char* _password) :
    io_service(_io_service),  socket(_io_service), id(_id), password(_password) {
      this->session_started = false;
    }

    bool connect(void);
    bool close(void);
    bool read(void);
    bool send(std::string);
    char* get_data(void);
    bool start_session(const char*);
    void assert_data_eq(const char*);

    int id;
    const char *password;
    boost::asio::io_service& io_service;
    boost::asio::ip::tcp::socket socket;
    char data[CLIENT_BUFFER_MAX_LENGTH];
    domoio::crypto::BlockCipher *block_cipher;
    bool session_started;

    bool send_raw(const char *, int);
    bool send_crypted(const char *, int);
  };




  // FACTORIES
  extern std::map<int, Device*> devices;
  Device *factory_device(int, const char*, const char*);

}

#endif //DOMOIO_TEST_H
