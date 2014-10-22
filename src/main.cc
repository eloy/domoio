#include "domoio.h"
#include "jsengine.h"

void on_exit() {
  domoio::exit_domoio();
}


void my_handler(int s){
  LOG(fatal) << "Caught signal:" << s;
  on_exit();
  exit(1);
}


int main(int argc, char* argv[]) {

  // Catch signals
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = my_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);

  // Init Domoio

  domoio::setup_config_options();
  domoio::prepare_config(argc, argv);

  domoio::init_domoio();

  // Exit callback
  std::atexit(on_exit);
  domoio::events::start();

  // JS service
  domoio::init_jsengine();

  domoio::run_server();
}
