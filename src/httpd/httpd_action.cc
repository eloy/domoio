#include "httpd.h"

#define PARAM_NAME_REGEXP ":([\\w_-]*)"
#define PARAM_VALUE_REGEXP "(\\\\d*)"

namespace domoio {
  namespace httpd {

    boost::regex param_name_regexp(PARAM_NAME_REGEXP);

    void HttpdAction::route_to_expression() {
      // Extract parameters
      boost::sregex_iterator iter(this->route.begin(), this->route.end(), param_name_regexp);
      boost::sregex_iterator end;
      for( int i=1; iter != end; ++iter) {
        boost::match_results<std::string::const_iterator> what = *iter;
        this->params_index[i++] = what[1];
      }

      // Build regexp
      std::string regexp_str = boost::regex_replace(this->route, param_name_regexp, PARAM_VALUE_REGEXP);
      this->regexp.assign(regexp_str);
    }

  }
}
