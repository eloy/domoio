#include <cstdlib>
#include <iostream>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "database.h"
#include "models.h"

#define CLIENT_BUFFER_MAX_LENGTH 1024

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
  private:
    boost::asio::ip::tcp::socket socket;
    char data[CLIENT_BUFFER_MAX_LENGTH];

    void handle_read(const boost::system::error_code&, size_t );
    void handle_write(const boost::system::error_code&);
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

}
