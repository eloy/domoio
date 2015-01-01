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
  }
}
