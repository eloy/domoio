#include "domoio.h"
#include "database.h"
#include "config.h"
#include "domoio_server.h"
#include "jsengine.h"
#include "httpd.h"
#include <sys/resource.h>

static bool EnableCoreDumps(void) {
  struct rlimit   limit;

  limit.rlim_cur = RLIM_INFINITY;
  limit.rlim_max = RLIM_INFINITY;
  return setrlimit(RLIMIT_CORE, &limit) == 0;
}



void exit_callback(int s) {
  LOG(info) << "Closing domoio server";
  domoio::exit_domoio();
  exit(1);
}


int main(int argc, char* argv[]) {
  // EnableCoreDumps();
  // Catch signals
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = exit_callback;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);

  // Init Domoio

  domoio::setup_config_options();
  domoio::prepare_config(argc, argv);

  //  domoio::init_ssdp();

  domoio::init_domoio();

  domoio::DeviceState::load_virtual_devices();

  domoio::events::start();

  // JS service
  domoio::init_jsengine();

  // HTTP Server
  domoio::httpd::init_httpd();

  // Devices Server
  domoio::run_server();
}
