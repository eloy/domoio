#include "domoio.h"


int main(int argc, char* argv[]) {
  domoio::db::connect();
  domoio::run();
  domoio::run_server();
  // while(true) {
  //   sleep(1000);
  // }
}
