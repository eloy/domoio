#include "domoio.h"


namespace domoio {

  // Constructor
  DeviceConnection::DeviceConnection(boost::asio::io_service& io_service) : socket(io_service) {
    this->device = 0;
    this->block_cipher = 0;
    this->session_started = false;
    this->logged_in = false;
  }

  // Destructor
  DeviceConnection::~DeviceConnection(void) {
    if (this->device != 0) {
      this->unregister_device_signals();
    }

    if (this->block_cipher != 0) {
      delete this->block_cipher;
    }
  }

  boost::asio::ip::tcp::socket& DeviceConnection::get_socket(void) {
    return socket;
  }

  void DeviceConnection::start(){
    this->send("Hey, protocol=1.0");
    this->read();
  }



  // Read and Write
  //--------------------------------------------------------------------
  bool DeviceConnection::send(std::string msg) {
    BOOST_LOG_TRIVIAL(trace) << "sending: '"<< msg << "'";
    if (this->session_started) {
      return this->send_crypted(msg.c_str(), msg.size());
    } else {
      return this->send_raw(msg.c_str(), msg.size());
    }
  }

  bool DeviceConnection::send_crypted(const char* str, int length) {
    // unsigned char * enc = this->block_cipher->encrypt(str, &length);
    // char *hex = domoio::crypto::hex_encode(enc, &length);
    // this->send_raw(hex, length);
    // free(enc);
    // free(hex);
    // return true;

    // DISABLE ENCRYPTATION
    char *hex = domoio::crypto::hex_encode((unsigned char*)str, &length);
    this->send_raw(hex, length);
    free(hex);
    return true;
  }

  bool DeviceConnection::send_raw(const char* msg, int length) {

    bool write_in_progress = !this->message_queue.empty();
    this->message_queue.push_back(std::string(msg, length));
    if (!write_in_progress) {
      this->write();
    }
    return true;
  }



  void DeviceConnection::read() {
    memset(data, 0, CLIENT_BUFFER_MAX_LENGTH);
    this->socket.async_read_some(boost::asio::buffer(data, CLIENT_BUFFER_MAX_LENGTH),
                                 boost::bind(&DeviceConnection::handle_read, this,
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));

  }

  void DeviceConnection::write() {
    std::stringstream stream;
    stream << this->message_queue.front().data() << "\n";
    int length = this->message_queue.front().length() + 1;
    boost::asio::async_write(socket,
                             boost::asio::buffer(stream.str(), length),
                             boost::bind(&DeviceConnection::handle_write, this, boost::asio::placeholders::error)
                             );

  }

  bool DeviceConnection::close() {
    this->socket.close();
    return true;
  }

  // Callbacks
  //--------------------------------------------------------------------


  void DeviceConnection::handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
    if (((boost::asio::error::eof == error) || (boost::asio::error::connection_reset == error)) && !disconnected) {
      this->disconnected = true;
      BOOST_LOG_TRIVIAL(trace) << "Device Disconnected\n";
    }

    if (error) { return ; }

    this->process_input(&this->data[0], bytes_transferred);
    this->read();
  }



  void DeviceConnection::handle_write(const boost::system::error_code& error) {
    if (error) { return ; }
    this->message_queue.pop_front();
    if (!this->message_queue.empty()) {
      this->write();
    }
  }

  // Process input
  //--------------------------------------------------------------------

  void DeviceConnection::process_input(const char* input_data, int bytes_transferred) {
    // If session not started, dispatch clean data
    if (!this->session_started) {
      this->dispatch_request(input_data);
      return;
    }

    // Decrypt if session started
    try {
      // int len = bytes_transferred - 1;
      // unsigned char *crypted = domoio::crypto::hex_decode(input_data, &len);
      // char * clean = this->block_cipher->decrypt(crypted, &len);

      // std::string str(clean, len);
      // this->dispatch_request(str);
      // free(crypted);
      // free(clean);

      // DISABLE ENCRYPTATION
      int len = bytes_transferred - 1;
      unsigned char *clean = domoio::crypto::hex_decode(input_data, &len);

      std::string str((char*)clean, len);
      this->dispatch_request(str);
      free(clean);

    }
    catch (std::exception& e) {
      BOOST_LOG_TRIVIAL(trace) << "Error decoding input: " << e.what() << "\n";
      this->session_started = false;
      this->send("400 Bad Request");
      this->close();
    }

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
    this->block_cipher = new domoio::crypto::BlockCipher(this->device->password);
    this->send(this->block_cipher->session_string().c_str());
    this->session_started = true;
    return true;

  }

  bool DeviceConnection::login(const char * passwd) {
    // TODO: Implement this
    if (strcmp(passwd, "1234") == 0) {
      this->logged_in = true;

      // Connect to signals
      this->register_device_signals();
      BOOST_LOG_TRIVIAL(trace) << "Device logged in: " << this->device->id;

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


  // Signals
  //-------------------------------------------------------------------

  void DeviceConnection::register_device_signals(void) {
    BOOST_LOG_TRIVIAL(trace) << "Registering signals device: " << this->device->id;
    this->device_signals_conn = this->device->network_signals.connect(boost::bind(&DeviceConnection::on_device_signal, this,_1));
  }

  void DeviceConnection::unregister_device_signals(void) {
    this->device_signals_conn.disconnect();
  }

  void DeviceConnection::on_device_signal(std::string str) {
    BOOST_LOG_TRIVIAL(trace) << "Signal: " << str << "\n";
    this->send(str);
  }

}
