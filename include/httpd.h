#include "mongoose.h"
#include <boost/regex.hpp>
#include "log.h"
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#ifndef HTTPD_H
#define HTTPD_H

#define HTTPD_PROCESS 5


namespace domoio {
  namespace httpd {

    class Request {
    public:
      Request() {};
      Request(struct mg_connection *_conn) : conn(_conn), method(_conn->request_method), url(_conn->uri) {}
      Request(const char *m, const char *u) : method(m), url(u) {}
      ~Request();
      const char *method;
      const char *url;

      int status;
      bool response_data(std::string data, int status=200) {
        mg_send_data(this->conn, data.c_str(), data.length());
        mg_send_status(this->conn, status);
        return true;
      }

      bool response_json(std::string data) {
        mg_send_header(this->conn, "Content-Type", "application/json");
        this->response_data(data);
        return true;
      }

      std::string post_data_raw() {
        return std::string(conn->content, conn->content_len);
      }

      void send_ws(std::string data) { mg_websocket_write(this->conn, WEBSOCKET_OPCODE_TEXT, data.c_str(), data.size()); }
      bool resolve(void);
      const char* param(std::string);
      int param_int(std::string);


      bool is_post() { return strcmp(this->method, "POST") == 0; }
      bool is_get() { return strcmp(this->method, "GET") == 0; }
      bool is_put() { return strcmp(this->method, "PUT") == 0; }
      bool is_options() { return strcmp(this->method, "OPTIONS") == 0; }
      bool is_delete() { return strcmp(this->method, "DELETE") == 0; }

      // std::map<std::string, std::string> request_headers;
    private:
      struct mg_connection *conn;
      bool has_url_params;
      std::map<std::string, std::string> params;
    };



    class WebSocketSession {
    public:
      WebSocketSession(Request *_request) : request(_request)  {
        boost::thread thread(boost::bind(&WebSocketSession::start, this));
      }

      ~WebSocketSession() {
        this->stop();
        delete(this->request);
      }

      virtual void init() {LOG(error) << "BAD_METHOD:" << request->url << " [" << request->method << "]"; }
      virtual void stop() {}
      virtual void handle_data(std::string data) {}
      void send(std::string data) { request->send_ws(data); }

    private:
      Request *request;
      void start() { this->init(); }
    };



    /**
     * HttpdAction
     */
    typedef bool (*HttpdCallback)(Request*);
    typedef WebSocketSession* (*WebSocketCallback)(Request*);

    class HttpdAction {
    public:
      HttpdAction(const char* _route, HttpdCallback cb) : route(_route), callback(cb) {
        route_to_expression();
      }
      HttpdCallback callback;
      std::string  route;
      boost::regex regexp;
      std::map<int, std::string> params_index;
    protected:
      HttpdAction(const char* _route) : route(_route) {
        route_to_expression();
      }
    private:
      void route_to_expression();
    };



    class WebSocketAction : public HttpdAction {
    public:
      WebSocketAction(const char *_route, WebSocketCallback cb) : HttpdAction(_route), websocket_callback(cb) {}
      WebSocketCallback websocket_callback;
    };


    bool init_httpd(void);
    bool stop_httpd(void);
    HttpdAction *find_action(const char *);
    bool register_action(const char*, HttpdCallback);


  }
}

#define DEF_HTTPD_ACTION(action_name) bool action_name(Request *request)
#define DEF_WS_ACTION(action_name) WebSocketSession* action_name(Request *request)
#endif //HTTPD_H
