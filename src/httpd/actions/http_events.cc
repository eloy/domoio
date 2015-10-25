#include "log.h"
#include "events.h"
#include "httpd.h"

#define BUFFER_SIZE 3072

namespace domoio {
  namespace httpd {
    namespace actions {

      class SSEContext{
      public:
        SSEContext() {
          this->run = true;
          this->events_connection = domoio::events::add_listener(boost::bind(&SSEContext::add_event, this, _1));
        }
        ~SSEContext() {
          this->events_connection.disconnect();
        }

        void add_event(EventPtr event) {
          LOG(error) << "Event Received";
          this->queue.push_back(event);
          boost::lock_guard<boost::mutex> lock(this->mutex);
          this->cond.notify_one();
        }

        void stop() {
          LOG(error) << "Close WS";
          this->run = false;
          boost::lock_guard<boost::mutex> lock(this->mutex);
          this->cond.notify_one();
        }


        std::deque<EventPtr> queue;
        bool run;

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

          while(this->ctx->run) {
            while(this->ctx->queue.empty() && this->ctx->run) {
              this->ctx->cond.wait(lock);
            }

            if(!this->ctx->run) {
              return;
            }

            EventPtr event_ptr = this->ctx->queue.front();
            this->ctx->queue.pop_front();
            Event *event = event_ptr.get();
            std::string data = event->json;
            LOG(error) << "WS Sent";
            this->send(data);
          }
        }

        void stop() {
          this->ctx->stop();
          delete(this->ctx);
        }

        void handle_data(std::string data) {
        }
      private:
        SSEContext *ctx;
        bool run;
      };


      WebSocketSession *events(Request *request) {
        return new EventsSession(request);
      }

    }
  }
}
