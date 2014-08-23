#ifndef DOMOIO_H
#define DOMOIO_H

#include <cstdlib>
#include <iostream>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/tokenizer.hpp>
#include "database.h"
#include "models.h"
#include "crypt.h"

#define CLIENT_BUFFER_MAX_LENGTH 1024
#define SERVER_THREADS 1

int test_crypt(void);


namespace domoio {
  int run(void);

  /*
   * Domoio Server
   */

  class DeviceConnection {
  public:
    DeviceConnection(boost::asio::io_service&);
    boost::asio::ip::tcp::socket& get_socket(void);
    void start();
    bool send(std::string);
    void read();
    bool logged_in();
    bool close();
  private:
    boost::asio::ip::tcp::socket socket;
    char data[CLIENT_BUFFER_MAX_LENGTH];

    void handle_read(const boost::system::error_code&, size_t );
    void handle_write(const boost::system::error_code&);

    void dispatch_request();
    void login();

    Device *device;
  };


  class Server {
  public:
    Server(boost::asio::io_service&, short);

  private:
    void start_accept(void);
    void handle_accept(DeviceConnection*, const boost::system::error_code&);

    boost::asio::io_service& io_service;
    boost::asio::ip::tcp::acceptor acceptor;
  };

  bool run_server(void);
  bool start_server(void);
  bool stop_server(void);




  /*
   * Commands
   */
  typedef std::vector<std::string>* CommandParams;
  typedef void (*CommandCallback)(DeviceConnection*, CommandParams);

  int register_server_command(std::string, CommandCallback);
}



#endif //DOMOIO_H
