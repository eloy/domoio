#include "domoio_test.h"


namespace domoio {

  const char *SPECIFICATIONS = "{\"serial\":\"1234\",\"manufacturer\":\"0x01\",\"model\":\"0x01\",\"description\":\"Alargadera 4 puertos\",\"ports\":[{\"name\":\"dio01\",\"digital\":true,\"output\":true},{\"name\":\"dio02\",\"digital\":true,\"output\":true},{\"name\":\"dio03\",\"digital\":true,\"output\":true},{\"name\":\"dio04\",\"digital\":true,\"output\":true}]}\n";



  Device * factory_device(int id, const char* name, const char* passwd) {
    Device * d = new domoio::Device(1, name, SPECIFICATIONS, passwd);
    devices[id] = d;
    return d;
  }
}
