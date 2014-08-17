#include "domoio.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace domoio {

  std::map<int, Device*> devices;

  void load_devices(void) {
    PGresult   *res;
    res = db::exec("select * from devices");

    int rows = PQntuples(res);

    int id_idx = PQfnumber(res, "id");
    int label_idx = PQfnumber(res, "label");
    int specs_idx = PQfnumber(res, "specifications");
    int pass_idx = PQfnumber(res, "password");

    for(int i=0; i < rows; i++) {
      int id = atoi(PQgetvalue(res, i, id_idx));
      char *label = PQgetvalue(res, i, label_idx);
      char *specifications = PQgetvalue(res, i, specs_idx);
      char *password = PQgetvalue(res, i, pass_idx);
      devices[id] = new Device(id, label, specifications, password);
    }
    PQclear(res);

    // for (std::map<int, Device*>::iterator it = devices.begin(); it != devices.end(); ++it) {
    //   Device *d = it->second;
    //   printf("Device id: %d => %s\n", d->id, d->serial.c_str());
    // }

  }

  Device *device_find(int id) {
    return devices[id];
  }

  void free_devices(void) {
    for (std::map<int, Device*>::iterator it = devices.begin(); it != devices.end(); ++it) {
      delete(it->second);
    }
  }

  Device::Device(int id_, const char *label_, const char *specs, const char *password_)
    : id(id_), label(label_), password(password_) {
    this->parse_specifications(specs);
  }

  Device::~Device(void) {
    BOOST_FOREACH(keypair(Port*) c, this->ports) {
      delete c.second;
    }
  }

  void Device::parse_specifications(const char *specs) {
    using boost::property_tree::ptree;
    ptree pt;

    std::stringstream input_str(specs);
    read_json(input_str, pt);

    this->serial = pt.get<std::string>("serial");
    this->manufacturer_id = pt.get<std::string>("manufacturer");
    this->description = pt.get<std::string>("description");
    this->model = pt.get<std::string>("model");

    BOOST_FOREACH(ptree::value_type &v, pt.get_child("ports")) {
      Port *port = new Port(v.second.get<std::string>("name"), v.second.get<bool>("digital"), v.second.get<bool>("output"));
      this->ports[port->name()] = port;
    }
  }

}
