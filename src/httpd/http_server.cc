#include "domoio.h"
#include "httpd.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

namespace domoio {
  namespace httpd {

    std::vector<HttpdAction*> httpd_actions;

    bool register_action(const char* regexp_str, HttpdCallback callback) {
      HttpdAction *action = new HttpdAction(regexp_str, callback);
      httpd_actions.push_back(action);
      return true;
    }

    namespace actions {
      DEF_HTTPD_ACTION(devices);
      DEF_HTTPD_ACTION(events);
    }

    void register_actions() {
      register_action("/api/devices/?:id?", &actions::devices);
      register_action("/api/events", &actions::events);
    }


    HttpdAction *find_action(const char *url) {
      for(std::vector<HttpdAction*>::iterator it = httpd_actions.begin(); it != httpd_actions.end(); ++it) {
        HttpdAction* action = *it;
        if (regex_match(url, action->regexp)) {
          return action;
        }
      }
      return NULL;
    }


    static int ahc_echo(void * cls, struct MHD_Connection *connection, const char *url, const char *method,
                        const char *version, const char *upload_data, size_t *upload_data_size, void **ptr) {

      static int dummy;

      int ret;

      LOG(info) << "Processing request type: " << method << " url: " << url << " version: " << version;

      /*
       * The first time only the headers are valid,
       * do not respond in the first round...
       */
      if (&dummy != *ptr) {
          *ptr = &dummy;
          return MHD_YES;
      }
      *ptr = NULL; /* clear context pointer */


      // Run callback
      Request request(method, url);
      if (request.resolve() == false) {
        return MHD_NO; /* unexpected method */
      }


      // // if (0 != *upload_data_size)
      // //   return MHD_NO; /* upload data in a GET!? */


      // Add global headers
      MHD_add_response_header(request.response, "Access-Control-Allow-Origin", "*");

      ret = MHD_queue_response(connection, request.status, request.response);
      MHD_destroy_response(request.response);
      return ret;
    }

    int init_httpd() {
      register_actions();
      int port = 8081;
      struct MHD_Daemon * d;
      d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, port, NULL, NULL, &ahc_echo, NULL, MHD_OPTION_END);
      if (d == NULL) return 1;
      // (void) getc ();
      // MHD_stop_daemon(d);
      return 0;
    }
  }
}
