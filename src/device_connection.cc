#include "domoio.h"


namespace domoio {


  DeviceConnection::~DeviceConnection(void) {
    if (this->device != 0) {
      delete this->device;
    }

    if (this->block_cipher != 0) {
      delete this->block_cipher;
    }
  }

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


  // Register Server Commands
  std::map<std::string, CommandCallback> server_commands;
  int register_server_command(std::string name, CommandCallback command) {
    server_commands[name] = command;
    return 1;
  }


  bool DeviceConnection::create_session(int device_id) {
    this->device = device_find(device_id);
    if (!this->device) {
      this->send("406 Not Acceptable");
      return false;
    }

    // Create Block Cipher
    this->block_cipher = new domoio::crypto::BlockCypher(this->device->password);
    this->send(this->block_cipher->session_string().c_str());
    this->session_started = true;
    return true;

  }

  // Dispatch Request from devices
  void DeviceConnection::dispatch_request() {
    // Tokenize command
    std::string str(this->data);
    boost::char_separator<char> separator(" ");
    boost::tokenizer<boost::char_separator<char> > tok(str, separator);
    std::vector<std::string> params;

    for(boost::tokenizer<boost::char_separator<char> >::iterator beg=tok.begin(); beg!=tok.end();++beg){
      params.push_back(*beg);
    }

    if (params.size() == 0) {
      this->send("400 Bad Request");
      return;
    }

    // Execute callback

    CommandCallback callback = server_commands[params[0]];
    if (callback == NULL) {
      printf("Invalid Command: '%s'\n", params[0].c_str());
      this->send("400 Bad Request");
      return;
    }

    callback(this, &params);
  }

}
