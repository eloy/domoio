#include "events.h"
#include "json.h"
#include <boost/thread.hpp>

namespace domoio {
  namespace events {


    boost::asio::io_service::id EventsService::id;

    boost::asio::io_service io_service;
    EventsService events_service(io_service);
    boost::asio::io_service::work work(io_service);


    // Shortcut for sending events
    void send(Event * event) {
      EventPtr event_ptr(event);
      events_service.send(event_ptr);
    }



    // Send the event through the io_service
    void EventsService::send(EventPtr event_ptr) {
      io_service.post(boost::bind(&EventsService::handle, this, event_ptr));
    }

    void EventsService::handle(EventPtr event_ptr) {
      this->event_signals(event_ptr);
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


  void Event::initialize() {
    this->date = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());
    this->json = this->to_json();
  }


  std::string Event::to_json() {
    json::Object ev;

    switch(this->type) {
    case events::port_set:
      ev["type"] = json::String("port_set");
      ev["device_id"] = json::Number(this->device->id);
      ev["port_id"] = json::Number(this->port->id());

      // If port is digital set to true or false
      if (this->port->digital()) {
        // new value
        if (this->new_value > 0) {
          ev["value"] = json::Boolean(true);
        } else {
          ev["value"] = json::Boolean(false);
        }
        // old value
        if (this->old_value > 0) {
          ev["old_value"] = json::Boolean(true);
        } else {
          ev["old_value"] = json::Boolean(false);
        }
      }
      // Otherwhise, just print the value
      else {
        ev["value"] = json::Number(this->new_value);
        ev["old_value"] = json::Number(this->old_value);
      }

      break;

      // Device Connected
    case events::device_connected:
      ev["type"] = json::String("device_connected");
      ev["device_id"] = json::Number(this->device->id);
      break;

      // Device Disconnected
    case events::device_disconnected:
      ev["type"] = json::String("device_disconnected");
      ev["device_id"] = json::Number(this->device->id);
      break;

    default:
      ev["type"] = json::String("unknown");
    }

    // Shared fields
    ev["date"] = json::String("foo");
    //pt.put("date", this->date);
    ev["channel"] = json::String(this->channel_name());


    std::stringstream ss;
    json::Writer::Write(ev, ss);
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
