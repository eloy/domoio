#include "domoio.h"

namespace domoio {
  void init_domoio(void) {
    BOOST_LOG_TRIVIAL(trace) << "A trace severity message";

    domoio::db::connect();
    domoio::load_devices();

    domoio::crypto::init();
  }



  void exit_domoio(void) {

  }
}
