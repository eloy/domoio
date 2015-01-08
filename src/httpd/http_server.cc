#include "domoio.h"
#include "httpd.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define POST_BUFFER_SIZE 2048

namespace domoio {
  namespace httpd {

    std::vector<HttpdAction*> httpd_actions;

    bool register_action(const char* regexp_str, HttpdCallback callback) {
      HttpdAction *action = new HttpdAction(regexp_str, callback);
      httpd_actions.push_back(action);
      return true;
    }

    namespace actions {
      DEF_HTTPD_ACTION(events);
      DEF_HTTPD_ACTION(devices);
      DEF_HTTPD_ACTION(ports);
      DEF_HTTPD_ACTION(set_port);
      DEF_HTTPD_ACTION(users);
    }

    void register_actions() {
      register_action("/api/events", &actions::events);

      register_action("/api/devices/?:id?", &actions::devices);
      register_action("/api/devices/:device_id/ports/?:id?", &actions::ports);
      register_action("/api/devices/:device_id/ports/:id/set", &actions::set_port);

      register_action("/api/users/?:id?", &actions::users);
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


    static int extract_headers (void *cls, enum MHD_ValueKind kind, const char *key, const char *value) {
      Request *request = (Request*) cls;
      request->request_headers[key] = value;
      return MHD_YES;
    }


    /*
     * Executed when request finish
     */
    void request_completed (void *cls, struct MHD_Connection *connection, void **con_cls, enum MHD_RequestTerminationCode toe) {
      LOG(trace) << "HTTP connection closed";
      if (NULL == cls) return;
      Request *request = (Request*) cls;

      if (request->require_post_processor()) {
        MHD_destroy_post_processor(request->post_processor);
      }

      delete(request);
      *con_cls = NULL;
    }


    static int iterate_post (void *cls, enum MHD_ValueKind kind, const char *key,
                             const char *filename, const char *content_type,
                             const char *transfer_encoding, const char *data,
                             uint64_t off, size_t size)
    {

      LOG(error) << "POST: " << data;

      // if (0 == strcmp (key, "name")) {
      //   if ((size > 0) && (size <= MAXNAMESIZE))
      //       {
      //         char *answerstring;
      //         answerstring = malloc (MAXANSWERSIZE);
      //         if (!answerstring) return MHD_NO;

      //         snprintf (answerstring, MAXANSWERSIZE, greatingpage, data);
      //         con_info->answerstring = answerstring;
      //       }
      //     else con_info->answerstring = NULL;

      //     return MHD_NO;
      // }

      return MHD_YES;
    }


    static int request_callback(void * cls, struct MHD_Connection *connection, const char *url, const char *method,
                                const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls) {

      Request *request;

      if (strcmp(method, "OPTIONS") == 0) {
        struct MHD_Response *resp = MHD_create_response_from_buffer(0, (void*)"", MHD_RESPMEM_MUST_COPY);
        MHD_add_response_header(resp, "Access-Control-Allow-Origin", "*");
        MHD_add_response_header(resp, "Access-Control-Allow-Methods", "GET, PUT, POST");
        MHD_add_response_header(resp, "Access-Control-Allow-Headers", "Content-type");
        int ret = MHD_queue_response(connection, 200, resp);
        MHD_destroy_response(resp);
        return ret;
      }



      /*
       * The first time only the headers are valid,
       * do not respond in the first round...
       */
      if (NULL == *con_cls) {
        LOG(trace) << "Processing request type: " << method << " url: " << url;
        request = new Request(method, url);

        // Extract request headers
        MHD_get_connection_values (connection, MHD_HEADER_KIND, &extract_headers, (void*) request);

        // Prepare POST data
        if (request->require_post_processor()) {
          LOG(trace) << "Adding POST processor";
          request->post_processor = MHD_create_post_processor(connection, POST_BUFFER_SIZE, iterate_post, (void*) request);
          if (NULL == request->post_processor) {
            LOG(warning) << "Can't create post processor";
            return MHD_NO;
          }
        }

        *con_cls = (void*) request;
        return MHD_YES;
      }

      /*
       * Second round, do the job
       */

      request = (Request*) *con_cls;

      // Process POST data
      if (*upload_data_size != 0) {
        request->post_data_received = true;
        request->post_data_raw << std::string(upload_data, *upload_data_size);
        if (request->require_post_processor()) {
          MHD_post_process(request->post_processor, upload_data, *upload_data_size);
        }

        *upload_data_size = 0;
        return MHD_YES;
      }

      // Run action
      if (request->resolve() == false) {
        return MHD_NO; /* unexpected method */
      }

      // Add global headers
      MHD_add_response_header(request->response, "Access-Control-Allow-Origin", "*");

      // And send resoponse
      int ret = MHD_queue_response(connection, request->status, request->response);
      MHD_destroy_response(request->response);

      return ret;
    }


    struct MHD_Daemon * daemon;
    bool init_httpd() {
      register_actions();
      LOG(info) << "Starting HTTP server on port " << conf_opt::http_port;
      daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, conf_opt::http_port, NULL, NULL,
                           &request_callback, NULL,
                           MHD_OPTION_NOTIFY_COMPLETED, &request_completed, NULL,
                           MHD_OPTION_END);
      return daemon != NULL;
    }

    bool stop_httpd() {
      MHD_stop_daemon(daemon);
      return true;
    }
  }
}
