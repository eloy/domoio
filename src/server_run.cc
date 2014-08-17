#include "domoio.h"

namespace domoio {

  boost::asio::io_service io_service;
  boost::thread_group m_threads;

  void run_io_service() { io_service.run(); }

  bool run_server(void) {
    try {
      Server server(io_service, 8013);
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
  bool start_server(void) {
    try {
      if (server == 0) {
        server = new Server(io_service, 8013);
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
      std::cerr << "Exception: " << e.what() << "\n";
      return false;
    }
  }


  bool stop_server(void) {
    io_service.stop();
    m_threads.join_all();
    return true;
  }
}
