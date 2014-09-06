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
      LOG << "Connection Started\n";
      device_connection->start();
    } else {
      delete device_connection;
    }
    start_accept();
  }


}
