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
    this->block_cipher = 0;
    this->session_started = false;
    this->logged_in = false;
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
    if (this->session_started) {
      return this->send_crypted(msg.c_str(), msg.size());
    } else {
      return this->send_raw(msg.c_str(), msg.size());
    }
  }

  bool DeviceConnection::send_crypted(const char* str, int length) {
    unsigned char * enc = this->block_cipher->encrypt(str, &length);
    char *hex = domoio::crypto::hex_encode(enc, &length);
    this->send_raw(hex, length);
    free(enc);
    free(hex);
    return true;
  }

  bool DeviceConnection::send_raw(const char* str, int length) {
    boost::asio::async_write(socket,
                             boost::asio::buffer(str, length + 1),
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

  // Callbacks
  //--------------------------------------------------------------------


  void DeviceConnection::handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
    if (!error) {
      // Decrypt if session started
      if (this->session_started) {
        try {
          int len = bytes_transferred - 1;
          unsigned char *crypted = domoio::crypto::hex_decode(&this->data[0], &len);
          char * clean = this->block_cipher->decrypt(crypted, &len);

          std::string str(clean);
          this->dispatch_request(str);
          free(crypted);
          free(clean);
        }
        catch (std::exception& e) {
          LOG << e.what();
          this->send("400 Bad Request");
          this->close();
        }
      }
      // Or read clean text
      else {
        this->dispatch_request(&this->data[0]);
      }

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


  // Dispatch Request from devices
  void DeviceConnection::dispatch_request(std::string str) {
    // Tokenize command
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


  // Session Management
  //--------------------------------------------------------------------


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

  bool DeviceConnection::login(const char * passwd) {
    // TODO: Implement this
    if (strcmp(passwd, "1234") == 0) {
      this->logged_in = true;
      return true;
    } else {
      return false;
    }
  }

  // Flags
  //--------------------------------------------------------------------

  bool DeviceConnection::is_logged_in() {
    return this->logged_in;
  }
  bool DeviceConnection::is_session_started() {
    return this->session_started;
  }
}
