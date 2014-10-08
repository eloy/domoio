#ifndef DOMOIO_H
#define DOMOIO_H
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <deque>
#include <time.h>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <boost/thread.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#define BOOST_LOG_DYN_LINK
#include <boost/log/trivial.hpp>
#include "config.h"
#include "exceptions.h"
#include "database.h"
#include "models.h"
#include "crypt.h"

#define CLIENT_BUFFER_MAX_LENGTH 1024
#define SERVER_THREADS 1

#define LOG BOOST_LOG_TRIVIAL

int test_crypt(void);



namespace domoio {


  int run(void);


  void init_domoio(void);
  void exit_domoio(void);
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
    Device* get_device() { return this->device; }

  private:
    boost::asio::ip::tcp::socket socket;
    char data[CLIENT_BUFFER_MAX_LENGTH];
    std::deque<std::string> message_queue;

    Device *device;
    domoio::crypto::BlockCipher *block_cipher;
    bool session_started;
    bool logged_in;
    bool disconnected;
    boost::signals2::connection device_signals_conn;


    bool send_raw(const char*, int);
    bool send_crypted(const char*, int);
    void handle_read(const boost::system::error_code&, size_t );
    void handle_write(const boost::system::error_code&);
    void process_input(const char*, int);
    void register_device_signals(void);
    void unregister_device_signals(void);
    void on_device_signal(std::string);
  };



  // Control Connection

  class ControlConnection : public Connection {
  public:
    ControlConnection(boost::asio::io_service&);
    ~ControlConnection();

    boost::asio::local::stream_protocol::socket& get_socket(void) { return socket; }
    void start();

    void read();
    bool close();
    bool send(std::string);

    bool create_session(int);
    bool login(const char *);
    bool execute_callback(std::vector<std::string>);
  private:
    boost::asio::local::stream_protocol::socket socket;
    char data[CLIENT_BUFFER_MAX_LENGTH];
    bool disconnected;

    void handle_read(const boost::system::error_code&, size_t );
    void handle_write(const boost::system::error_code&);
  };


  // Domoio Device Protocol Server

  class Server {
  public:
    Server(boost::asio::io_service&, short);

  private:
    void start_accept(void);
    void handle_accept(DeviceConnection*, const boost::system::error_code&);

    boost::asio::io_service& io_service;
    boost::asio::ip::tcp::acceptor acceptor;
  };


  // Control Server
  class ControlServer {
  public:
    ControlServer(boost::asio::io_service& , const std::string&);

  private:
    void start_accept(void);
    void handle_accept(ControlConnection*, const boost::system::error_code&);


    boost::asio::io_service& io_service;
    boost::asio::local::stream_protocol::acceptor acceptor;
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
  typedef void (*ControlCommandCallback)(ControlConnection*, CommandParams);

  class DeviceCommandDef {
  public:
    DeviceCommandDef(DeviceCommandCallback callback_, int argc_, std::string help_) : callback(callback_), argc(argc_), help(help_) {}

    DeviceCommandCallback callback;
    int argc;
    std::string help;
  };


  bool register_device_command(std::string, DeviceCommandDef*);

  class ControlCommandDef {
  public:
  ControlCommandDef(ControlCommandCallback callback_, int argc_, std::string help_) : callback(callback_), argc(argc_), help(help_) {}

    ControlCommandCallback callback;
    int argc;
    std::string help;
  };

  bool register_control_command(std::string, ControlCommandDef*);

}



#endif //DOMOIO_H
