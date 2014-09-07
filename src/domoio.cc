#include "domoio.h"

namespace domoio {
  void init_domoio(void) {
    domoio::db::connect();
    domoio::load_devices();

    domoio::crypto::init();
  }



  void exit_domoio(void) {
    remove(conf_opt::socket_path.c_str());
  }
}
