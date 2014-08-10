#include "domoio_test.h"

TEST(Devices, load_devices) {
  domoio::load_devices();
  domoio::free_devices();
}
