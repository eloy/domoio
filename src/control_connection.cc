#include "domoio.h"

namespace domoio {
  ControlConnection::ControlConnection(boost::asio::io_service& io_service) : socket(io_service) {
  }



  void ControlConnection::start(){
    LOG(trace) << "Started Control Connection";
    this->send("Hey, protocol=1.0\n");
    this->read();
  }


  void ControlConnection::read() {
    memset(data, 0, CLIENT_BUFFER_MAX_LENGTH);
    this->socket.async_read_some(boost::asio::buffer(data, CLIENT_BUFFER_MAX_LENGTH),
                                 boost::bind(&ControlConnection::handle_read, this,
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));

  }


  bool ControlConnection::send(std::string msg) {
    boost::asio::async_write(socket,
                             boost::asio::buffer(msg.c_str(), msg.length()),
                             boost::bind(&ControlConnection::handle_write, this, boost::asio::placeholders::error)
                             );
    return true;

  }

  void ControlConnection::handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
    if (error) { return; }

    std::string cmd(&this->data[0], bytes_transferred);
    this->dispatch_request(cmd);
  }

  void ControlConnection::handle_write(const boost::system::error_code& error) {
    if (error) { return; }

    this->read();
  }


  bool ControlConnection::close() {
    LOG(trace) << "Control Connection Closed";
    this->socket.close();
    return true;
  }


  void ControlConnection::send_event(Event* event) {
    this->send("event_data " + event->channel_name() + " " + event->to_json());
  }


  bool ControlConnection::create_session(int user_id) {
    return true;
  }
  bool ControlConnection::login(const char * passwd) {
    return true;
  }
}
