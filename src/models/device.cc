#include "domoio.h"

namespace domoio {

  std::map<int, Device*> devices;

  void load_devices(void) {
    PGresult   *res;
    res = db::exec("select * from devices");

    int rows = PQntuples(res);

    int id_idx = PQfnumber(res, "id");
    int label_idx = PQfnumber(res, "label");
    int specs_idx = PQfnumber(res, "specifications");

    for(int i=0; i < rows; i++) {
      int id = atoi(PQgetvalue(res, i, id_idx));
      char *label = PQgetvalue(res, i, label_idx);
      char *specifications = PQgetvalue(res, i, specs_idx);
      devices[id] = new Device(id, label, specifications);
    }
    PQclear(res);

    // for (std::map<int, Device*>::iterator it = devices.begin(); it != devices.end(); ++it) {
    //   Device *d = it->second;
    //   printf("Device id: %d => %s\n", d->id, d->label.c_str());
    // }

  }


  void free_devices(void) {
    for (std::map<int, Device*>::iterator it = devices.begin(); it != devices.end(); ++it) {
      delete(it->second);
    }
  }


  Device::Device(int id_, const char *label_, const char *specs) : id(id_), label(label_) {

  }
}
