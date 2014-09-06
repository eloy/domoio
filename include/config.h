#ifndef CONFIG_H
#define CONFIG_H
#include <boost/program_options.hpp>


#ifndef DOMOIO_CONFIG_FILE
// #define DOMOIO_CONFIG_FILE "/etc/domoio/config"
#define DOMOIO_CONFIG_FILE "config"
#endif

namespace domoio {

  namespace conf_opt {
    extern int port;
  }
  // Config
  extern boost::program_options::variables_map conf;
  void setup_config_options(void);
  void prepare_config(int, char*[]);

}

#endif //CONFIG_H
