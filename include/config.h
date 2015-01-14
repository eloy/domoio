#include <boost/program_options.hpp>
#ifndef CONFIG_H
#define CONFIG_H



#ifndef DOMOIO_CONFIG_FILE
// #define DOMOIO_CONFIG_FILE "/etc/domoio/config"
#define DOMOIO_CONFIG_FILE "config"
#endif

namespace domoio {

  namespace conf_opt {
    extern int domoio_port;
    extern int http_port;

    // Config file
    extern std::string config_file_name;

    // DATABASE
    extern std::string db_name;
    extern std::string db_user;
    extern std::string db_password;
    extern std::string db_host;

    // JS
    extern std::string js_system_path;
    extern std::string js_user_path;
  }
  // Config
  extern boost::program_options::variables_map conf;
  void setup_config_options(const char *default_config_file=DOMOIO_CONFIG_FILE);
  void prepare_config(int, char*[]);

}

#endif //CONFIG_H
