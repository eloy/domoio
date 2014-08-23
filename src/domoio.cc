#include "domoio.h"

namespace domoio {
  void init_domoio(void) {
    domoio::db::connect();
    domoio::crypto::init();
  }

}
