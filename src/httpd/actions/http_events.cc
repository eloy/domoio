#include "log.h"
#include "events.h"
#include "httpd.h"
#include <boost/thread.hpp>

#define BUFFER_SIZE 3072

namespace domoio {
  namespace httpd {
    namespace actions {

      class SSEContext{
      public:
        SSEContext() {
          this->events_connection = domoio::events::add_listener(boost::bind(&SSEContext::add_event, this, _1));
        }
        ~SSEContext() {
          this->events_connection.disconnect();
        }

        void add_event(EventPtr event) {
          this->queue.push_back(event);
          boost::lock_guard<boost::mutex> lock(this->mutex);
          this->cond.notify_one();
        }

        std::deque<EventPtr> queue;
        bool initialized;

        boost::condition_variable cond;
        boost::mutex mutex;

      private:
        signals_connection events_connection;
      };



      class EventsSession : public WebSocketSession {
      public:
        EventsSession(Request *_request) : WebSocketSession(_request) {}
        virtual void init() {
          LOG(error) << "Init ws";
          this->ctx = new SSEContext();

          boost::unique_lock<boost::mutex> lock(this->ctx->mutex);
          while(this->ctx->queue.empty()) {
            this->ctx->cond.wait(lock);
          }

          EventPtr event_ptr = this->ctx->queue.front();
          this->ctx->queue.pop_front();
          Event *event = event_ptr.get();
          std::string data = event->json;
          this->send(data);
        }

        void stop() {
          delete(this->ctx);
        }

        void handle_data(std::string data) {
        }
      private:
        SSEContext *ctx;
      };


      WebSocketSession *events(Request *request) {
        return new EventsSession(request);
      }

    }
  }
}
