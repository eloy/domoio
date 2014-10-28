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
      date = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());
    }

    Event(events::type _type, events::channel _channel, int _old_value) :
      type(_type), channel(_channel), old_value(_old_value) {
      date = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());
    }

    Event(events::type _type, events::channel _channel, Device*  _device) :
      type(_type), channel(_channel), device(_device) {
      date = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());
    }

    Event(events::type _type, events::channel _channel, int _old_value, int _new_value) :
      type(_type), channel(_channel), old_value(_old_value), new_value(_new_value) {
      date = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());
    }

    Event(events::type _type, events::channel _channel, Device *_device, Port*_port, int _old_value, int _new_value) :
      type(_type), channel(_channel), device(_device), port(_port), old_value(_old_value), new_value(_new_value) {
      date = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time());
    }

    std::string to_json();
    std::string channel_name();

    events::type type;
    events::channel channel;
    Port *port;
    Device *device;
    int old_value;
    int new_value;
    std::string date;
  };

}


#endif //EVENTS_H