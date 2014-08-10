#include "domoio_test.h"

const char *SPECIFICATIONS = "{\"serial\":\"1234\",\"manufacturer\":\"0x01\",\"model\":\"0x01\",\"description\":\"Alargadera 4 puertos\",\"ports\":[{\"name\":\"dio01\",\"digital\":true,\"output\":true},{\"name\":\"dio02\",\"digital\":true,\"output\":true},{\"name\":\"dio03\",\"digital\":true,\"output\":true},{\"name\":\"dio04\",\"digital\":true,\"output\":true}]}\n";

TEST(Devices, load_devices) {
  domoio::load_devices();
  domoio::free_devices();
}


TEST(Devices, DeviceConstructor) {
  domoio::Device device (1, "test device", SPECIFICATIONS);
  EXPECT_EQ(device.serial, "1234");
  EXPECT_EQ(device.ports_count(), 4);

  domoio::Port *port = device.port("dio03");
  EXPECT_EQ(port->digital(), true);
}
