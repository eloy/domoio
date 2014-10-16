#include "domoio.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

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
      LOG(trace) << "Event: " << event->type;
      this->event_signals(event);
      delete(event);
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

  std::string Event::to_json() {
    using boost::property_tree::ptree;
    ptree pt;

    switch(this->type) {
    case events::port_set:
      pt.put("type", "port_set");
      pt.put("device_id", this->device->id);
      pt.put("port_id", this->port->id());
      pt.put("value", this->new_value);
      pt.put("old_value", this->old_value);
      break;

    // Device Connected
    case events::device_connected:
      pt.put("type", "device_connected");
      pt.put("device_id", this->device->id);
      break;

    // Device Disconnected
    case events::device_disconnected:
      pt.put("type", "device_disconnected");
      pt.put("device_id", this->device->id);
      break;

    default:
      pt.put("type", "unkuwn");
    }

    // Shared fields
    pt.put("date", this->date);
    pt.put("channel", this->channel_name());


    std::stringstream ss;
    write_json(ss, pt);
    return ss.str();
  }

  std::string Event::channel_name() {
    switch(this->channel) {
    case events::private_channel:
      return "private";
      break;
    case events::public_channel:
      return "public";
      break;
    case events::restricted_channel:
      return "restricted";
      break;
    default:
      return "unkuwn";
    }
  }

}
