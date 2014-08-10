#include "domoio_test.h"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::Environment* const env = ::testing::AddGlobalTestEnvironment(new domoio::TestEnvironment);

  return RUN_ALL_TESTS();
}
