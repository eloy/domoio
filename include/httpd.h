#include <microhttpd.h>
#include <boost/regex.hpp>
#ifndef HTTPD_H
#define HTTPD_H



namespace domoio {
  namespace httpd {

    class Request {
    public:
      Request(const char* _method, const char *_url) : method(_method), url(_url) {}
      ~Request();
      const char *method;
      const char *url;
      int status;
      struct MHD_Response *response;
      bool resolve(void);
      const char* param(std::string);
      int param_int(std::string);
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


    int init_httpd(void);
    HttpdAction *find_action(const char *);
    bool register_action(const char*, HttpdCallback);
  }
}
#define DEF_HTTPD_ACTION(action_name) bool action_name(Request *request)
#endif //HTTPD_H
