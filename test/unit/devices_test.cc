#include "domoio_test.h"

const char *SPECIFICATIONS = "{\"serial\":\"1234\",\"manufacturer\":\"0x01\",\"model\":\"0x01\",\"description\":\"Alargadera 4 puertos\",\"ports\":[{\"id\":0,\"name\":\"dio01\",\"digital\":true,\"output\":true},{\"id\":1,\"name\":\"dio02\",\"digital\":true,\"output\":true},{\"id\":2,\"name\":\"dio03\",\"digital\":true,\"output\":true},{\"id\":3,\"name\":\"dio04\",\"digital\":true,\"output\":true}]}\n";

  TEST(Devices, load_devices) {
  domoio::load_devices();
  domoio::free_devices();
}


TEST(Devices, DeviceConstructor) {
  domoio::NetworkDevice device (1, "test device", SPECIFICATIONS, "foo");
  EXPECT_EQ(device.serial, "1234");
  EXPECT_EQ(device.ports_count(), 4);
}


TEST(Devices, DeviceFind) {
  domoio::NetworkDevice *m_device_1 = domoio::factory_network_device(1, "foo", "password");
  domoio::NetworkDevice *m_device_2 = domoio::factory_network_device(2, "bar", "password");

  domoio::Device * device_1 = domoio::device_find(1);
  EXPECT_EQ(device_1->id, 1);

  domoio::Device * device_2 = domoio::device_find(2);
  EXPECT_EQ(device_2->id, 2);

  delete(m_device_1);
  delete(m_device_2);
}
