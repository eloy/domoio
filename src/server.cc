#include "domoio.h"

namespace domoio {
  Server::Server(boost::asio::io_service& _io_service, short _port) :
    io_service(_io_service), acceptor(_io_service,  boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), _port)) {
    start_accept();
  }


  void Server::start_accept() {
    DeviceConnection* device_connection = new DeviceConnection(io_service);
    acceptor.async_accept(device_connection->get_socket(),
                           boost::bind(&Server::handle_accept, this, device_connection,
                                       boost::asio::placeholders::error));
  }

  void Server::handle_accept(DeviceConnection* device_connection, const boost::system::error_code& error) {
    if (!error) {
      device_connection->start();
    } else {
      delete device_connection;
    }
    start_accept();
  }


  boost::asio::io_service io_service;
  bool run_server(void) {
    try {
      Server  server(io_service, 8013);
      io_service.run();
      // boost::asio::io_service::work work(io_service);
    }
    catch (std::exception& e) {
      std::cerr << "Exception: " << e.what() << "\n";
      return false;
    }

    return true;
  }


  bool start_server(void) {
    boost::thread server_thread(run_server);
    return true;
  }


  bool stop_server(void) {
    io_service.stop();
    return true;
  }
}
