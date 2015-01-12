#include "models.h"
#include "domoio_test.h"


namespace domoio {

  const char *VIRTUAL_DEVICE = "{\"specifications\":{\"serial\":\"123\",\"manufacturer\":\"0x01\",\"model\":\"0x02\",\"description\":\"Test Device\",\"ports\":[{\"id\":0,\"name\":\"dio01\",\"digital\":true,\"output\":false},{\"id\":1,\"name\":\"dio02\",\"digital\":true,\"output\":false},{\"id\":2,\"name\":\"dio03\",\"digital\":true,\"output\":false},{\"id\":3,\"name\":\"dio04\",\"digital\":true,\"output\":false}]},\"config\":null,\"label\":\"\",\"password\":null,\"type\":\"VirtualDevice\"}";

  const char *NETWORK_DEVICE = "{\"specifications\":{\"serial\":\"123\",\"manufacturer\":\"0x01\",\"model\":\"0x02\",\"description\":\"Test Device\",\"ports\":[{\"id\":0,\"name\":\"dio01\",\"digital\":true,\"output\":false},{\"id\":1,\"name\":\"dio02\",\"digital\":true,\"output\":false},{\"id\":2,\"name\":\"dio03\",\"digital\":true,\"output\":false},{\"id\":3,\"name\":\"dio04\",\"digital\":true,\"output\":false}]},\"config\":null,\"label\":\"\",\"password\":null,\"type\":\"NetworkDevice\"}";

  void load_device( Device *device, const char* label, const char* passwd, bool virt) {
    if (virt) {
      device->from_json(VIRTUAL_DEVICE);
    } else {
      device->from_json(NETWORK_DEVICE);
    }
    device->label.assign(label);
    device->password.assign(passwd);
  }

  Device * factory_device(const char* label, const char* passwd, bool virt) {
    Device *device = new Device();
    load_device(device, label, passwd, virt);
    return device;
  }

}
