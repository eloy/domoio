#include "domoio_test.h"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::Environment* const env = ::testing::AddGlobalTestEnvironment(new domoio::TestEnvironment);
  domoio::setup_config_options();
  domoio::prepare_config(argc, argv);
  return RUN_ALL_TESTS();
}
