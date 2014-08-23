#include "domoio.h"


int main(int argc, char* argv[]) {
  // JS service
  // domoio::run();
  domoio::init_domoio();
  domoio::run_server();
}
