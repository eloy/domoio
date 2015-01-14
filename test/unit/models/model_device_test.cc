#include <gtest/gtest.h>
#include "devices.h"
#include "models.h"


TEST(DeviceModel, save) {
  const char *specs = "{\"serial\":\"1234\",\"manufacturer\":\"0x01\",\"model\":\"0x01\",\"description\":\"Alargadera 4 puertos\",\"ports\":[{\"id\":0,\"name\":\"dio01\",\"digital\":true,\"output\":true},{\"id\":1,\"name\":\"dio02\",\"digital\":true,\"output\":true},{\"id\":2,\"name\":\"dio03\",\"digital\":true,\"output\":true},{\"id\":3,\"name\":\"dio04\",\"digital\":true,\"output\":true}]}";

  const char *raw = "{\"id\":2,\"specifications\":{\"serial\":\"4543\",\"manufacturer\":\"0x01\",\"model\":\"0x02\",\"description\":\"Virtual Device\",\"ports\":[{\"id\":0,\"name\":\"dio01\",\"digital\":true,\"output\":false},{\"id\":1,\"name\":\"dio02\",\"digital\":true,\"output\":false},{\"id\":2,\"name\":\"dio03\",\"digital\":true,\"output\":false},{\"id\":3,\"name\":\"dio04\",\"digital\":true,\"output\":false}]},\"config\":null,\"label\":\"Botonera\",\"password\":null,\"created_at\":\"2014-10-12T17:52:58.295Z\",\"updated_at\":\"2014-10-12T17:55:04.554Z\"}";

  json::Object doc;
  std::stringstream in_ss(raw);
  json::Reader::Read(doc, in_ss);

  std::stringstream ss;
  json::Writer::Write(doc, ss);

  domoio::Device device;
  device.from_json(raw);

}
