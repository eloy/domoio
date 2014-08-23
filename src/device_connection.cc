#include "domoio.h"


namespace domoio {

  DeviceConnection::DeviceConnection(boost::asio::io_service& io_service) : socket(io_service) {
    this->device = 0;
  }

  boost::asio::ip::tcp::socket& DeviceConnection::get_socket(void) {
    return socket;
  }

  void DeviceConnection::start(){
    this->send("Hey, protocol=1.0\n");
    this->read();
  }



  // Read and Write
  //--------------------------------------------------------------------

  bool DeviceConnection::send(std::string msg) {
    boost::asio::async_write(socket,
                             boost::asio::buffer(msg.c_str(), msg.length() + 1),
                             boost::bind(&DeviceConnection::handle_write, this, boost::asio::placeholders::error)
                             );
    return true;
  }

  void DeviceConnection::read() {
    memset(data, 0, CLIENT_BUFFER_MAX_LENGTH);
    this->socket.async_read_some(boost::asio::buffer(data, CLIENT_BUFFER_MAX_LENGTH),
                                 boost::bind(&DeviceConnection::handle_read, this,
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));

  }


  bool DeviceConnection::close() {
    this->socket.close();
    return true;
  }

  // Flags
  //--------------------------------------------------------------------

  bool DeviceConnection::logged_in() {
    return this->device != 0;
  }

  void DeviceConnection::handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
    if (!error) {
      this->dispatch_request();
    } else {
      // delete this;
    }
  }



  void DeviceConnection::handle_write(const boost::system::error_code& error) {
    if (!error) {
      this->read();
    } else {
      // delete this;
    }
  }

  bool starts_with(const char *str, const char *pre) {
    return strncmp(pre, str, strlen(pre)) == 0;
  }


  void DeviceConnection::login() {
    if (starts_with(data, "login")) {
      std::string str(this->data);
      boost::tokenizer<> tok(str);
      std::vector<std::string> params;

      for(boost::tokenizer<>::iterator beg=tok.begin(); beg!=tok.end();++beg){
        params.push_back(*beg);
      }

      int id = atoi(params.at(1).c_str());
      Device *device = device_find(id);
      std::cout << params.at(2) << "\n";

      if (false) {
      } else {
        this->send("403 Forbidden");
      }
    } else {
      this->send("401 Unauthorized");
    }
  }

  std::map<std::string, CommandCallback> server_commands;
  int register_server_command(std::string name, CommandCallback command) {
    return 1;
  }

  void DeviceConnection::dispatch_request() {
    // if (this->logged_in()) {

    // } else {
    //   this->login();
    // }

    std::string str(this->data);
    boost::tokenizer<> tok(str);
    std::vector<std::string> params;

    for(boost::tokenizer<>::iterator beg=tok.begin(); beg!=tok.end();++beg){
      params.push_back(*beg);
    }

    if (params.size() == 0) {
      this->send("400 Bad Request");
      return;
    }

    CommandCallback callback = server_commands[params[0]];
    if (callback == NULL) {
      this->send("400 Bad Request");
      return;
    }

    callback(this, &params);
  }

}
