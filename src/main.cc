#include "domoio.h"


int main(int argc, char* argv[]) {

  domoio::setup_config_options();
  domoio::prepare_config(argc, argv);

  // JS service
  // domoio::run();
  domoio::init_domoio();
  domoio::run_server();
}
