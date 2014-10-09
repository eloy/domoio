#ifndef EVENTS_H
#define EVENTS_H

#define EVENT_HANDLER_THREADS 1

namespace domoio {
  typedef boost::signals2::connection signals_connection;

  class Event {
  public:
    Event() {}

    Event(std::string _name) : name(_name) {}

    Event(std::string _name, int _old_value) :
      name(_name), old_value(_old_value) {
    }

    Event(std::string _name, int _old_value, int _new_value) :
      name(_name), old_value(_old_value), new_value(_new_value) {
    }

    Event(std::string _name, Device *_device, Port*_port, int _old_value, int _new_value) :
      name(_name), device(_device), port(_port), old_value(_old_value), new_value(_new_value) {
    }

    std::string to_json();

    std::string name;
    Port *port;
    Device *device;
    int old_value;
    int new_value;
  };


  namespace events {

    void send(Event *);
    bool start();
    bool stop();

    class EventsService : public boost::asio::io_service::service {
    public:

      /// The unique service identifier.
      static boost::asio::io_service::id id;

      EventsService(boost::asio::io_service& io_service) : boost::asio::io_service::service(io_service) {

      }

      ~EventsService() {

      }

      void shutdown_service() {
      }

      void send(Event*);

      void handle(Event*);

      template<typename CompletionHandler> signals_connection add_listener(CompletionHandler handler) {
        return this->event_signals.connect(handler);
      }

    private:
      boost::signals2::signal<void(Event*)> event_signals;
    };

    extern EventsService events_service;
    template<typename CompletionHandler> signals_connection add_listener(CompletionHandler handler) {
      return events_service.add_listener(handler);
    }
  }
}


#endif //EVENTS_H
