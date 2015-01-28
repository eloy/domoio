#include "domoio.h"
//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#define SSDP_PORT 1900
#define SSDP_ADDRESS "239.255.255.250"

namespace domoio {


  using boost::asio::ip::udp;

  std::string build_response() {
    std::string str = "HTTP/1.1 200 OK\r\n";
    str += "CACHE-CONTROL: max-age=120\r\n";
    str += "ST: upnp:rootdevice\r\n";
    str +=" USN: uuid:73796E6F-6473-6D00-0000-0011321a9fb8::upnp:rootdevice\r\n";
    str += "EXT:\r\n";
    str += "SERVER: Synology/DSM/10.254.0.16\r\n";
    str += "LOCATION: http://10.254.0.16:8081/ssdp.xml\r\n";
    str += "OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n";
    str += "01-NLS: 1\r\n";
    // str += "BOOTID.UPNP.ORG: 1\r\n";
    // str += "CONFIGID.UPNP.ORG: 1337\r\n\r\n";

    return str;
  }

  class udp_server {
  public:
    udp_server(boost::asio::io_service& io_service) : socket_(io_service) {
      const boost::asio::ip::address& multicast_address = boost::asio::ip::address::from_string(SSDP_ADDRESS);
      boost::asio::ip::address ipAddr = boost::asio::ip::address_v4::any();
      boost::asio::ip::udp::endpoint listen_endpoint(ipAddr, SSDP_PORT);

      socket_.open(listen_endpoint.protocol());


      socket_.set_option(boost::asio::socket_base::broadcast(true));
      socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
      socket_.bind(listen_endpoint);

      // Join the multicast group.
      socket_.set_option(boost::asio::ip::multicast::join_group(multicast_address));

      start_receive();
    }

  private:
    void start_receive() {
      socket_.async_receive_from(
                                 boost::asio::buffer(recv_buffer_), remote_endpoint_,
                                 boost::bind(&udp_server::handle_receive, this,
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));
    }

    void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred) {
      std::string message(&recv_buffer_[0], bytes_transferred);
      LOG(error) << "UDP message received: " << message;

      if (!error || error == boost::asio::error::message_size)
        {
          boost::shared_ptr<std::string> message(new std::string(build_response()));


          socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
                                boost::bind(&udp_server::handle_send, this, message,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));

          start_receive();
        }
    }

    void handle_send(boost::shared_ptr<std::string> /*message*/,
                     const boost::system::error_code& /*error*/,
                     std::size_t /*bytes_transferred*/)
    {
    }

    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    boost::array<char, 1024> recv_buffer_;
  };

  int init_ssdp()
  {
    try
      {
        boost::asio::io_service io_service;
        udp_server server(io_service);
        io_service.run();
      }
    catch (std::exception& e)
      {
        std::cerr << e.what() << std::endl;
      }

    return 0;
  }
}
