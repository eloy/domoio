#include "domoio.h"
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>

namespace domoio {
  void init_domoio(void) {
    namespace logging = boost::log;
    logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::warning);

    domoio::db::connect();
    domoio::load_devices();

    domoio::crypto::init();
  }



  void exit_domoio(void) {
    domoio::events::stop();
    remove(conf_opt::socket_path.c_str());
  }
}
