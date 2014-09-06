#include "domoio.h"

namespace domoio {
  void init_domoio(void) {
    domoio::db::connect();
    domoio::load_devices();

    domoio::crypto::init();
  }

}
