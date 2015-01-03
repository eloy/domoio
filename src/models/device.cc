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
    int type_idx = PQfnumber(res, "type");
    int pass_idx = PQfnumber(res, "password");
    for(int i=0; i < rows; i++) {
      int id = atoi(PQgetvalue(res, i, id_idx));
      char *label = PQgetvalue(res, i, label_idx);
      char *specifications = PQgetvalue(res, i, specs_idx);
      char *type = PQgetvalue(res, i, type_idx);
      if (strcmp(type, "NetworkDevice") == 0) {

        char *password = PQgetvalue(res, i, pass_idx);
        devices[id] = new NetworkDevice(id, label, specifications, password);
      }
      else if (strcmp(type, "VirtualDevice") == 0) {
        devices[id] = new VirtualDevice(id, label, specifications);
      }
    }

    PQclear(res);

    // for (std::map<int, Device*>::iterator it = devices.begin(); it != devices.end(); ++it) {
    //   Device *d = it->second;
    //   printf("Device id: %d => %s\n", d->id, d->serial.c_str());
    // }

  }


  std::map<int, Device*>::iterator devices_iterator() {
    return devices.begin();
  }

  const std::map<int, Device*> *all_devices() {
    return &devices;
  }
  Device *device_find(int id) {
    return devices[id];
  }

  NetworkDevice * NetworkDevice::find(int id) {
    Device *d = device_find(id);
    if (d == NULL || !d->is_network_device()) {
      return NULL;
    }
    return (NetworkDevice*) d;
  }

  void free_devices(void) {
    for (std::map<int, Device*>::iterator it = devices.begin(); it != devices.end(); ++it) {
      delete(it->second);
    }
  }


  void Device::parse_specifications(std::string specs) {
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

    port->set_value(value);
    return true;
  }

  void Device::to_json_object(json::Object &d) {

    d["id"] = json::Number(this->id);
    d["label"] = json::String(this->label);
    d["description"] = json::String(this->description);
    d["connected"] = json::Boolean(this->connected);
    json::Array ports;
    for (std::map<int, Port*>::iterator it = this->ports.begin(); it != this->ports.end(); ++it) {
      json::Object port;
      it->second->to_json_object(port);
      ports.Insert(port);
    }
    d["ports"] = ports;
  }

  bool device_set_port_state(std::string target_url, int value) {
    int device_id;
    int port_id;

    sscanf(target_url.c_str(), "%d::%d", &device_id, &port_id);
    Device *device = device_find(device_id);
    if (device == NULL)  return false;
    if (device->is_connected() == false) return false;
    return device->set_port_state(port_id, value);
  }


  std::string devices_to_json() {
    json::Array devices_array;
    for (std::map<int, Device*>::iterator it = devices.begin(); it != devices.end(); ++it) {
      json::Object device;
      it->second->to_json_object(device);
      devices_array.Insert(device);
    }

    std::stringstream ss;
    json::Writer::Write(devices_array, ss);
    return ss.str();
  }

}
