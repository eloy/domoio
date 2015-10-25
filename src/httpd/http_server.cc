#include "domoio.h"
#include "httpd.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define POST_BUFFER_SIZE 4096

namespace domoio {
  namespace httpd {

    std::vector<HttpdAction*> httpd_actions;
    std::vector<WebSocketAction*> websocket_actions;

    bool register_httpd_action(const char* regexp_str, HttpdCallback callback) {
      HttpdAction *action = new HttpdAction(regexp_str, callback);
      httpd_actions.push_back(action);
      return true;
    }

    bool register_ws_action(const char* regexp_str, WebSocketCallback callback) {
      WebSocketAction *action = new WebSocketAction(regexp_str, callback);
      websocket_actions.push_back(action);
      return true;
    }

    namespace actions {
      DEF_WS_ACTION(events);
      // DEF_HTTPD_ACTION(console);
      DEF_HTTPD_ACTION(devices);
      DEF_HTTPD_ACTION(ports);
      DEF_HTTPD_ACTION(set_port);
      DEF_HTTPD_ACTION(users);

    }

    void register_httpd_actions() {
      // Event Source server
      register_ws_action("/api/events", &actions::events);
      // register_httpd_action("/api/console/?:id?", &actions::console);

      // API
      register_httpd_action("/api/devices/?:id?", &actions::devices);
      register_httpd_action("/api/devices/:device_id/ports/?:id?", &actions::ports);
      register_httpd_action("/api/devices/:device_id/ports/:id/set", &actions::set_port);

      register_httpd_action("/api/users/?:id?", &actions::users);

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

    WebSocketAction *find_ws_action(const char *url) {
      for(std::vector<WebSocketAction*>::iterator it = websocket_actions.begin(); it != websocket_actions.end(); ++it) {
        WebSocketAction* action = *it;
        if (regex_match(url, action->regexp)) {
          return action;
        }
      }
      return NULL;
    }



    static bool handle_request(struct mg_connection *conn) {
      LOG(error) << "Accepting request: " << conn->uri;
      Request request(conn);
      return request.resolve();
    }

    static int ev_handler(struct mg_connection *conn, enum mg_event ev) {
      // Authorization
      if (ev == MG_AUTH) return MG_TRUE;

      // WebSockets CONNECT
      if(ev == MG_WS_CONNECT) {
        WebSocketAction *action = find_ws_action(conn->uri);
        if(action == NULL) return MG_FALSE;

        Request *request = new Request(conn);
        conn->connection_param = action->websocket_callback(request);
        return MG_TRUE;
      }

      if (ev == MG_CLOSE && conn->is_websocket) {
        LOG(error) << "Close Event";
        WebSocketSession *action = (WebSocketSession *)conn->connection_param;
        delete(action);
        return MG_TRUE;
      }

      if (ev == MG_REQUEST) {
        if (conn->is_websocket) {
          return MG_TRUE;
        }

        if (handle_request(conn)) {
          return MG_TRUE;
        }
      }

      return MG_FALSE;
    }



    class ServerInstance {
    public:
      struct mg_server *server;
      pthread_t thread_id;
    };

    ServerInstance *instances[HTTPD_PROCESS];


    static void *server_loop(void *server) {
      for (;;) mg_poll_server((struct mg_server *) server, 1000);
      return NULL;
    }

    bool init_httpd() {
      register_httpd_actions();
      LOG(info) << "Starting HTTP server on port " << conf_opt::http_port;

      for (int i=0; i < HTTPD_PROCESS; i++) {
        instances[i] = new ServerInstance();
        instances[i]->server = mg_create_server(NULL, ev_handler);
        mg_set_option(instances[i]->server, "listening_port", "8081");
        mg_set_option(instances[i]->server, "document_root", "/Users/harlock/src/c/domoio/web/public");

        instances[i]->thread_id = (pthread_t) mg_start_thread(server_loop, instances[i]->server);
      }
      return true;
    }

    bool stop_httpd() {
      // Cleanup, and free server instance
      for (int i=0; i < HTTPD_PROCESS; i++) {
        pthread_cancel(instances[i]->thread_id);
        mg_destroy_server(&instances[i]->server);
        delete(instances[i]);
      }
      return true;
    }
  }
}
