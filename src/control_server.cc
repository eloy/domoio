#include "domoio.h"
#include <cstdio>
#include <iostream>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

namespace domoio {
  ControlServer::ControlServer(boost::asio::io_service& _io_service, const std::string& file)
    : io_service(_io_service),
      acceptor(_io_service, boost::asio::local::stream_protocol::endpoint(file)) {
    this->start_accept();
  }


  void ControlServer::start_accept() {
    ControlConnection* conn = new ControlConnection(io_service);
    acceptor.async_accept(conn->get_socket(),
                          boost::bind(&ControlServer::handle_accept, this, conn,
                                      boost::asio::placeholders::error));
  }

  void ControlServer::handle_accept(ControlConnection* conn, const boost::system::error_code& error) {
    if (!error) {
      LOG(info) << "Local Connection Started\n";
      conn->start();
    } else {
      delete conn;
    }
    start_accept();
  }


}



// class session
//   : public boost::enable_shared_from_this<session>
// {
// public:
//   session(boost::asio::io_service& io_service)
//     : socket_(io_service)
//   {
//   }

//   stream_protocol::socket& socket()
//   {
//     return socket_;
//   }

//   void start()
//   {
//     socket_.async_read_some(boost::asio::buffer(data_),
//                             boost::bind(&session::handle_read,
//                                         shared_from_this(),
//                                         boost::asio::placeholders::error,
//                                         boost::asio::placeholders::bytes_transferred));
//   }

//   void handle_read(const boost::system::error_code& error,
//                    size_t bytes_transferred)
//   {
//     if (!error)
//       {
//         boost::asio::async_write(socket_,
//                                  boost::asio::buffer(data_, bytes_transferred),
//                                  boost::bind(&session::handle_write,
//                                              shared_from_this(),
//                                              boost::asio::placeholders::error));
//       }
//   }

//   void handle_write(const boost::system::error_code& error)
//   {
//     if (!error)
//       {
//         socket_.async_read_some(boost::asio::buffer(data_),
//                                 boost::bind(&session::handle_read,
//                                             shared_from_this(),
//                                             boost::asio::placeholders::error,
//                                             boost::asio::placeholders::bytes_transferred));
//       }
//   }

// private:
//   // The socket used to communicate with the client.
//   stream_protocol::socket socket_;

//   // Buffer used to store data received from the client.
//   boost::array<char, 1024> data_;
// };
