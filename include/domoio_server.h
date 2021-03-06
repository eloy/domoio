#include <deque>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "crypt.h"
#include "events.h"

#ifndef DOMOIO_SERVER_H
#define DOMOIO_SERVER_H

#define CLIENT_BUFFER_MAX_LENGTH 1024
#define SERVER_THREADS 1

namespace domoio {
  /*
   * Domoio Server
   */

  // Connection

  class Connection {
  public:
    void dispatch_request(std::string);

    virtual bool create_session(int) { return false; }
    virtual bool send(std::string) { return false; }
    virtual bool close() { return false; }
    virtual bool login(const char *) {return false; }
    virtual bool execute_callback(std::vector<std::string>) { return false; }
  };


  // Device Connection

  class DeviceConnection : public Connection,  public boost::enable_shared_from_this<DeviceConnection> {
  public:
    DeviceConnection(boost::asio::io_service&);
    ~DeviceConnection();

    boost::asio::ip::tcp::socket& get_socket(void);
    void start();
    bool send(std::string);
    void read();
    void write();
    bool login(const char *);
    bool is_logged_in();
    bool is_session_started();
    bool close();
    bool create_session(int);
    bool execute_callback(std::vector<std::string>);
    DeviceState* get_device() { return this->device; }

  private:
    boost::asio::ip::tcp::socket socket;
    char data[CLIENT_BUFFER_MAX_LENGTH];
    std::deque<std::string> message_queue;

    DeviceState *device;
    domoio::crypto::BlockCipher *block_cipher;
    bool session_started;
    bool logged_in;
    bool disconnected;
    signals_connection device_signals_conn;


    bool send_raw(const char*, int);
    bool send_crypted(const char*, int);
    void handle_read(const boost::system::error_code&, size_t );
    void handle_write(const boost::system::error_code&);
    void process_input(const char*, int);
    void register_device_signals(void);
    void unregister_device_signals(void);
    void on_device_signal(std::string);
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


  //-------------------------------------------------------------

  bool run_server(void);
  bool start_server(void);
  bool stop_server(void);




  /*
   * Commands
   */
  typedef std::vector<std::string>* CommandParams;
  typedef void (*DeviceCommandCallback)(DeviceConnection*, CommandParams);

  class DeviceCommandDef {
  public:
  DeviceCommandDef(DeviceCommandCallback callback_, int argc_, std::string help_) : callback(callback_), argc(argc_), help(help_) {}

    DeviceCommandCallback callback;
    int argc;
    std::string help;
  };


  bool register_device_command(std::string, DeviceCommandDef*);
  void register_device_commands();
  void unregister_device_commands();

}

#endif //DOMOIO_SERVER_H
