#include "models.h"
#include "devices.h"
#include "domoio.h"
#include <boost/foreach.hpp>

namespace domoio {

  using namespace vault;

  void Port::after_from_json_object(json::Object *doc) {
    json::Number port_id = (*doc)["id"];
    this->id = port_id.Value();
  }

  void Port::after_to_json_object(json::Object *doc) {
    // Force the id when it is 0
    if (this->id == 0) {
      (*doc)["id"] = json::Number(0);
    }
  }

  Specifications::~Specifications() {
    for(std::map<int, Port*>::iterator it = this->ports.begin(); it != this->ports.end(); ++it) {
      Port *port = it->second;
      delete(port);
    }
    this->ports.clear();
  }


  void Specifications::after_from_json_object(json::Object *doc) {
    json::Array ports = (*doc)["ports"];
    json::Array::const_iterator it(ports.Begin()), itEnd(ports.End());
    for (; it != itEnd; ++it) {
      const json::Object& doc = *it;
      Port *port = new Port();
      port->from_json_object(doc);
      this->ports[port->get_id()] = port;
    }
  }

  void Specifications::after_to_json_object(json::Object *doc) {
    json::Array ports;

    for(std::map<int, Port*>::iterator it = this->ports.begin(); it != this->ports.end(); ++it) {
      Port *port = it->second;
      ports.Insert(port->to_json_object());
    }
    (*doc)["ports"] = ports;
  }

  bool Device::after_load(PGresult *res, int row) {
    this->specifications.from_json(this->specifications_raw);
    return true;
  }

  bool Device::before_save() {
    this->specifications_raw.assign(this->specifications.to_json());
    return true;
  }

  void Device::after_from_json_object(json::Object *doc) {
    json::Object specs = (*doc)["specifications"];
    this->specifications.from_json_object(specs);
  }

  void Device::after_to_json_object(json::Object *doc) {
    (*doc)["specifications"] = this->specifications.to_json_object();

    DeviceState *state = DeviceState::find(this->get_id());
    bool connected = (state != NULL);
    (*doc)["connected"] = json::Boolean(connected);

    // Build ports with info from specifications, config and state
    json::Array ports;

    for(std::map<int, Port*>::iterator it = this->specifications.ports.begin(); it != this->specifications.ports.end(); ++it) {
      Port *port = it->second;
      json::Object obj = port->to_json_object();

      if (connected) {
        PortState *port_state = state->port(port->get_id());
        if (port_state != NULL) {
          port_state->to_json_object(obj);
        }
      }

      ports.Insert(obj);
    }

    (*doc)["ports"] = ports;


  }


  Port * Device::get_port(int port_id) {
    for(std::map<int, Port*>::iterator it = this->specifications.ports.begin(); it != this->specifications.ports.end(); ++it) {
      Port *port = it->second;
      if (port->get_id() == port_id) {
        return port;
      }
    }
    return NULL;
  }

  void Device::add_port(Port *port) {
    // port->set_id(this->specifications.ports.size());
    this->specifications.ports[port->get_id()] = port;
  }



  std::map<int, DeviceState*> DeviceState::active_devices;

  DeviceState::DeviceState(Device *device) : id(device->get_id()), is_virtual(device->is_virtual()) {
    // Build ports
    for(std::map<int, Port*>::iterator it = device->specifications.ports.begin(); it != device->specifications.ports.end(); ++it) {
      Port *port = it->second;
      PortState *port_state = new PortState(this, port->get_id(), port->digital, port->output);
      this->ports[port->get_id()] = port_state;
    }
  }

  DeviceState::~DeviceState(void) {
    for (std::map<int, PortState*>::iterator it = this->ports.begin(); it != this->ports.end(); ++it) {
      PortState *state = it->second;
      delete(state);
      this->ports.erase(it);
    }
  }


  DeviceState *DeviceState::connect(Device *device) {
    DeviceState *state = new DeviceState(device);
    DeviceState::active_devices[device->get_id()] = state;
    return state;
  }

  void DeviceState::disconnect(int device_id) {
    std::map<int,DeviceState *>::iterator it;
    it = DeviceState::active_devices.find(device_id);
    if (it == DeviceState::active_devices.end()) {
      return;
    }

    DeviceState *state = it->second;
    delete(state);
    DeviceState::active_devices.erase(it);
  }


  DeviceState *DeviceState::find(int device_id) {
    std::map<int,DeviceState *>::iterator it;
    it = DeviceState::active_devices.find(device_id);
    if (it == DeviceState::active_devices.end()) {
      return NULL;
    }
    return it->second;
  }


  PortState *DeviceState::port(int id) {
    std::map<int,PortState *>::iterator it;
    it = this->ports.find(id);
    if (it == this->ports.end()) {
      return NULL;
    }
    return it->second;
  }


  void DeviceState::load_virtual_devices(void) {
    ModelsCollection<Device> devices;
    Device::all(&devices);

    std::vector<Device*>::iterator it;
    for(it = devices.begin(); it != devices.end(); ++it) {
      Device *device = *it;
      if (device->is_virtual()) {
        DeviceState::active_devices[device->get_id()] = new DeviceState(device);
      }
    }
  }


  void DeviceState::unload_devices(void) {
    for (std::map<int, DeviceState*>::iterator it = DeviceState::active_devices.begin(); it != DeviceState::active_devices.end(); ++it) {
      DeviceState *state = it->second;
      delete(state);

    }
    DeviceState::active_devices.clear();
  }



}
