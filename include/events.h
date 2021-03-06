#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "devices.h"

#ifndef EVENTS_H
#define EVENTS_H

#define EVENT_HANDLER_THREADS 1

namespace domoio {
  typedef boost::signals2::connection signals_connection;
  class Event;
  typedef boost::shared_ptr<Event> EventPtr;

  namespace events {

    void send(Event *);
    bool start();
    bool stop();

    enum type {

      // Sent when a device change the state of a port
      port_set,
      // Sent when a device is connected
      device_connected,
      // Sent when a device is disconnected
      device_disconnected

    };
    // http://www.cmu.edu/iso/governance/guidelines/data-classification.html
    enum channel {
      public_channel,
      private_channel,
      restricted_channel
    };

    class EventsService : public boost::asio::io_service::service {
    public:

      /// The unique service identifier.
      static boost::asio::io_service::id id;

      EventsService(boost::asio::io_service& io_service) : boost::asio::io_service::service(io_service) {}
      ~EventsService() {}
      void shutdown_service() {}
      void send(EventPtr);

      void handle(EventPtr);

      template<typename CompletionHandler> signals_connection add_listener(CompletionHandler handler) {
        return this->event_signals.connect(handler);
      }

    private:
      boost::signals2::signal<void(EventPtr)> event_signals;
    };

    extern EventsService events_service;
    template<typename CompletionHandler> signals_connection add_listener(CompletionHandler handler) {
      return events_service.add_listener(handler);
    }
  }

  class Event {
  public:
    Event() {}

    Event(events::type _type, events::channel _channel) : type(_type), channel(_channel) {
      this->initialize();
    }

    Event(events::type _type, events::channel _channel, int _old_value) :
      type(_type), channel(_channel), old_value(_old_value) {
      this->initialize();
    }

    Event(events::type _type, events::channel _channel, DeviceState*  _device) :
      type(_type), channel(_channel), device(_device) {
      this->initialize();
    }

    Event(events::type _type, events::channel _channel, int _old_value, int _new_value) :
      type(_type), channel(_channel), old_value(_old_value), new_value(_new_value) {
      this->initialize();
    }

    Event(events::type _type, events::channel _channel, DeviceState *_device, PortState *_port, int _old_value, int _new_value) :
      type(_type), channel(_channel), device(_device), port(_port), old_value(_old_value), new_value(_new_value) {
      this->initialize();
    }

    std::string json;
    std::string channel_name();

    events::type type;
    events::channel channel;
    PortState *port;
    DeviceState *device;
    int old_value;
    int new_value;
    std::string date;

  private:
    void initialize();
    std::string to_json();
  };

}


#endif //EVENTS_H
