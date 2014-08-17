#include "domoio.h"


int main(int argc, char* argv[]) {
  // JS service
  // domoio::run();
  domoio::db::connect();
  domoio::run_server();
}
