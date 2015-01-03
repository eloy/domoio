#include "domoio.h"
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>

namespace domoio {
  void init_domoio(void) {
    namespace logging = boost::log;
    //logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::warning);
    logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);
    domoio::db::connect();
    domoio::load_devices();
    // Add commands
    domoio::init_control_commands();
    domoio::init_device_commands();

    domoio::crypto::init();
  }



  void exit_domoio(void) {
    domoio::events::stop();
    remove(conf_opt::socket_path.c_str());
  }
}
