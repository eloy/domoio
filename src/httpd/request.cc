#include "domoio.h"
#include "httpd.h"

namespace domoio {
  namespace httpd {

    Request::~Request() {
    }


    /*
     * Find action, extract params and run callback
     */
    bool Request::resolve(void) {
      HttpdAction *action = find_action(url);
      if (action == NULL) {
        return false;
      }

      // Extract params from url
      boost::cmatch matches;
      this->has_url_params = regex_search(url, matches, action->regexp);
      if (this->has_url_params) {
        for (int i=1; i < matches.size(); i++) {
          std::string name = action->params_index[i];
          this->params[name] = matches[i].str();
        }
      }

      return action->callback(this);
    }


    /*
     * Params
     */
    const char* Request::param(std::string name) {
      if (!this->has_url_params) return NULL;
      return this->params[name].c_str();
    }

    // TODO: Check errors
    int Request::param_int(std::string name) {
      const char* value = this->param(name);
      if (value == NULL) return 0;

      return atoi(value);
    }



    // std::string Request::param(std::string name) {
    //   int size = 255;
    //   char value[size];
    //   std::string out;
    //   int length = mg_get_var(this->conn, name.c_str(), &value[0], size);
    //   if (length > 0) {
    //     out.assign(&value[0], length);
    //   }
    //   return out;
    // }

    // // TODO: Check errors
    // int Request::param_int(std::string name) {
    //   std::string value = this->param(name);
    //   if (value.length() == 0) return 0;

    //   return atoi(value.c_str());
    // }


    // bool Request::require_post_processor() {
    //   if (this->request_headers["Content-Type"] == "application/x-www-form-urlencoded") {
    //     return true;
    //   }
    //   return false;
    // }

    // const char * CONTENT_TYPE_JSON = "application/json";
    // bool Request::has_json_post_data() {
    //   std::string content_type = this->request_headers["Content-Type"];
    //   return strncmp(CONTENT_TYPE_JSON, content_type.c_str(), strlen(CONTENT_TYPE_JSON));
    // }

  }
}
