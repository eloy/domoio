#include "domoio.h"


void on_exit() {
  domoio::exit_domoio();
}

int main(int argc, char* argv[]) {
  domoio::setup_config_options();
  domoio::prepare_config(argc, argv);

  // JS service
  // domoio::run();
  domoio::init_domoio();

  // Exit callback
  std::atexit(on_exit);

  domoio::run_server();
}
