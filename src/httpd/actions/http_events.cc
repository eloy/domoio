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

      static void free_callback(void *cls) {
        SSEContext *ctx = (SSEContext*) cls;
        delete(ctx);
      }

      static ssize_t data_generator (void *cls, uint64_t pos, char *buf, size_t buffer_size) {
        SSEContext *ctx = (SSEContext*) cls;
        if (ctx->initialized == false) {
          ctx->initialized = true;
          int initiator_size = 2048;
          char initiator[initiator_size];
          memset(&initiator[0], ' ', initiator_size);
          return snprintf(buf, buffer_size, ":%s\n\n", initiator);
        }

        boost::unique_lock<boost::mutex> lock(ctx->mutex);
        while(ctx->queue.empty()) {
          ctx->cond.wait(lock);
        }

        EventPtr event_ptr = ctx->queue.front();
        ctx->queue.pop_front();
        Event *event = event_ptr.get();
        std::string data = event->json;
        return snprintf(buf, buffer_size, "data: %s\n\n", data.c_str());

        return 0;
      }



      bool events(Request *request) {
        SSEContext *ctx = new SSEContext();
        request->status = MHD_HTTP_OK;
        request->response = MHD_create_response_from_callback (MHD_SIZE_UNKNOWN, BUFFER_SIZE, &data_generator, ctx, &free_callback);
        MHD_add_response_header(request->response, "Content-Type", "text/event-stream");
        MHD_add_response_header(request->response, "Cache-Control",  "no-cache");
        MHD_add_response_header(request->response,"Connection", "keep-alive");
        return true;
      }
    }
  }
}
