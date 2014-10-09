#include "domoio.h"

namespace domoio {

  boost::asio::io_service io_service;
  boost::thread_group m_threads;

  void run_io_service() { io_service.run(); }

  bool run_server(void) {
    try {
      LOG(trace) << "Starting server in port " << conf_opt::port << "\n";
      Server server(io_service, conf_opt::port);
      ControlServer control_server(io_service, conf_opt::socket_path);

      boost::asio::io_service::work work(io_service);
      io_service.run();
      // // Start servers
      // for(int i = 0; i < SERVER_THREADS; i++ ) {
      //   m_threads.create_thread(run_io_service);
      // }

      return true;
    }
    catch (std::exception& e) {
      std::cerr << "Exception: " << e.what() << "\n";
      return false;
    }
  }

  Server *server = 0;
  ControlServer *control_server = 0;

  bool start_network_servers(void) {
    try {
      if (server == 0 && control_server == 0) {
        server = new Server(io_service, conf_opt::port);
        control_server = new ControlServer(io_service, conf_opt::socket_path);
        boost::asio::io_service::work work(io_service);
      }
      else {
        io_service.reset();
      }

      // // Start servers
      for(int i = 0; i < SERVER_THREADS; i++ ) {
        m_threads.create_thread(run_io_service);
      }

      return true;
    }
    catch (std::exception& e) {
      LOG(fatal) << "Exception: " << e.what() << "\n";
      return false;
    }
  }


  bool stop_network_servers(void) {
    io_service.stop();
    m_threads.join_all();
    io_service.reset();
    return true;
  }



  bool start_server(void) {
    return events::start() && start_network_servers();
  }

  bool stop_server(void) {
    return stop_network_servers() && events::stop();
  }
}
