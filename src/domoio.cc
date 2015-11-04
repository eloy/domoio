#include "domoio_server.h"
#include "database.h"
#include "log.h"
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>

namespace domoio {
  void init_domoio(void) {
    namespace logging = boost::log;
    //logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::warning);
    logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);
    domoio::db::connect();
    // Add commands
    domoio::register_device_commands();

    domoio::crypto::init();
  }



  void exit_domoio(void) {
    domoio::crypto::exit();
    domoio::unregister_device_commands();
    domoio::events::stop();
    domoio::DeviceState::unload_devices();
    domoio::db::close();
  }
}
