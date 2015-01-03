#include <microhttpd.h>
#include <boost/regex.hpp>
#ifndef HTTPD_H
#define HTTPD_H



namespace domoio {
  namespace httpd {

    class Request {
    public:
      Request() {};
      Request(const char *m, const char *u) : method(m), url(u) {}
      ~Request();
      const char *method;
      const char *url;

      int status;
      bool response_data(std::string data, int status=MHD_HTTP_OK) {
        this->response = MHD_create_response_from_buffer(data.length(), (void*)data.c_str(), MHD_RESPMEM_MUST_COPY);
        this->status = status;
        return true;
      }

      bool response_json(std::string data) {
        this->response_data(data, MHD_HTTP_OK);
        MHD_add_response_header(this->response, "Content-Type", "application/json");
        return true;
      }

      struct MHD_Response *response;


      struct MHD_PostProcessor *post_processor;
      std::stringstream post_data_raw;
      bool post_data_received;

      bool resolve(void);
      const char* param(std::string);
      int param_int(std::string);


      bool is_post() { return strcmp(this->method, "POST") == 0; }
      bool is_get() { return strcmp(this->method, "GET") == 0; }
      bool is_put() { return strcmp(this->method, "PUT") == 0; }
      bool is_options() { return strcmp(this->method, "OPTIONS") == 0; }
      bool is_delete() { return strcmp(this->method, "DELETE") == 0; }

      bool require_post_processor();
      bool has_json_post_data();

      std::map<std::string, std::string> request_headers;
    private:
      bool has_url_params;
      std::map<std::string, std::string> params;
    };


    typedef bool (*HttpdCallback)(Request*);

    class HttpdAction {
    public:
      HttpdAction(const char* _route, HttpdCallback cb) : route(_route), callback(cb) {
        route_to_expression();
      }
      HttpdCallback callback;
      std::string  route;
      boost::regex regexp;
      std::map<int, std::string> params_index;
    private:
      void route_to_expression();
    };


    bool init_httpd(void);
    bool stop_httpd(void);
    HttpdAction *find_action(const char *);
    bool register_action(const char*, HttpdCallback);
  }
}

#define DEF_HTTPD_ACTION(action_name) bool action_name(Request *request)
#endif //HTTPD_H
