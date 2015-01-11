#include <gtest/gtest.h>
#include "devices.h"
#include "models.h"

TEST(DeviceModel, save) {
  const char *specs = "{\"serial\":\"1234\",\"manufacturer\":\"0x01\",\"model\":\"0x01\",\"description\":\"Alargadera 4 puertos\",\"ports\":[{\"id\":0,\"name\":\"dio01\",\"digital\":true,\"output\":true},{\"id\":1,\"name\":\"dio02\",\"digital\":true,\"output\":true},{\"id\":2,\"name\":\"dio03\",\"digital\":true,\"output\":true},{\"id\":3,\"name\":\"dio04\",\"digital\":true,\"output\":true}]}";

  domoio::Device device;
  device.label.assign("foo");
  device.specifications_raw.assign(specs);
  device.save();

  LOG(error) << device.to_json();
}
