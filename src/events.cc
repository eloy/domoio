#include "domoio.h"

namespace domoio {


  namespace events {


    boost::asio::io_service::id EventsService::id;

    boost::asio::io_service io_service;
    EventsService events_service(io_service);
    boost::asio::io_service::work work(io_service);

    void EventsService::send(Event * event) {
      io_service.post(boost::bind(&EventsService::handle, this, event));
    }

    void EventsService::handle(Event * event) {
      LOG(trace) << "Event: " << event->name;
      this->event_signals(event);
    }


    // Shortcut for sending events
    void send(Event * event) {
      events_service.send(event);
    }

    boost::thread_group m_threads;

    void run_io_service() { io_service.run(); }
    bool started = false;
    bool start(void) {
      try {
        if (!started) {
          started = true;
        }
        else {
          io_service.reset();
        }

        // // Start servers
        for(int i = 0; i < EVENT_HANDLER_THREADS; i++ ) {
          m_threads.create_thread(run_io_service);
        }

        return true;
      }
      catch (std::exception& e) {
        LOG(fatal) << "Exception: " << e.what() << "\n";
        return false;
      }
    }


    bool stop(void) {
      io_service.stop();
      m_threads.join_all();
      io_service.reset();
      return true;
    }

  }

}
