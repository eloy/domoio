#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include "domoio_server.h"
#include "devices.h"
#include "events.h"
#include "config.h"

#ifndef DOMOIO_TEST_H
#define DOMOIO_TEST_H

#define DOMOIO_ENV_TEST

namespace domoio {
  bool expect_events(int);

  class FeatureTest  : public testing::Test {
  protected:
    virtual void SetUp() {
      domoio::events::start();
      domoio::start_server();
    }
    virtual void TearDown() {
      domoio::events::stop();
      domoio::stop_server();
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

    ~TestDevice();

    bool connect(void);
    bool login(void);
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


  class TestControl {
  public:
    TestControl(boost::asio::io_service &_io_service) :
      io_service(_io_service),  socket(_io_service) {
    }

    TestControl(boost::asio::io_service& _io_service, int _id, const char* _password) :
      io_service(_io_service),  socket(_io_service), id(_id), password(_password) {
    }

    bool connect(void);

    bool close(void);
    bool read(void);
    bool send(std::string);
    char* data(void);

    int id;
    const char *password;

    boost::asio::io_service& io_service;
    boost::asio::local::stream_protocol::socket socket;


    char buffer[CLIENT_BUFFER_MAX_LENGTH];
  };


  // FACTORIES
  // extern std::map<int, Device*> devices;
  Device *factory_device(const char*, const char*, bool virt=false);

}

#endif //DOMOIO_TEST_H
