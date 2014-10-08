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
    this->connected = false;
    this->parse_specifications(specs);
  }

  Device::~Device(void) {
    for (std::map<int, Port*>::iterator it = this->ports.begin(); it != this->ports.end(); ++it) {
      delete(it->second);
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
      Port *port = new Port(this, v.second.get<int>("id"), v.second.get<std::string>("name"), v.second.get<bool>("digital"), v.second.get<bool>("output"));
      this->ports[port->id()] = port;
    }
  }


  bool Device::set_port_state(int port_id, int value) {
    LOG(trace) << "set_port_state:" << this->id;
    Port *port = this->port(port_id);
    if (port == NULL) return false;

    if (port->value() == value) {
      return true;
    }

    port->set_value(value);

    char buffer[CLIENT_BUFFER_MAX_LENGTH];
    snprintf(&buffer[0], CLIENT_BUFFER_MAX_LENGTH, "set %d %d", port_id, value);
    this->network_signals(&buffer[0]);

    // TODO send change event
    return true;
  }

  bool device_set_port_state(std::string target_url, int value) {
    int device_id;
    int port_id;

    sscanf(target_url.c_str(), "%d::%d", &device_id, &port_id);
    Device *device = device_find(device_id);
    if (device == NULL)  return false;

    return device->set_port_state(port_id, value);
  }

}
