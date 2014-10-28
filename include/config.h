#ifndef CONFIG_H
#define CONFIG_H
#include <boost/program_options.hpp>


#ifndef DOMOIO_CONFIG_FILE
// #define DOMOIO_CONFIG_FILE "/etc/domoio/config"
#define DOMOIO_CONFIG_FILE "config"
#endif



#ifndef DOMOIO_UNIX_SOCKET_PATH
#define DOMOIO_UNIX_SOCKET_PATH "/var/run/domoio.sock"
#endif
namespace domoio {

  namespace conf_opt {
    extern int port;
    extern std::string socket_path;

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
  void setup_config_options(void);
  void prepare_config(int, char*[]);

}

#endif //CONFIG_H
