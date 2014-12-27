#ifndef MODELS_H
#define MODELS_H
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "jsengine.h"
#define keypair(type) std::pair<std::string, type>

namespace domoio {

  class Device;

  class Port {
  public:
    Port(Device* _device, int id, std::string name, bool digital, bool output)
      : device(_device), _id(id), _name(name), _digital(digital), _output(output) {
      this->_value = -1;
    }
    std::string name() { return this->_name; };
    bool digital() { return this->_digital; }
    bool analogic() { return !this->_digital; }
    bool output() { return this->_output; }
    bool input() { return !this->_output; }
    int id() { return this->_id; }
    int value() { return this->_value; }
    void set_value(int);

    boost::property_tree::ptree to_pt();

  protected:
    int _id;
    std::string _name;
    bool _digital;
    bool _output;
    int _value;
    Device *device;
  };

  // class NetworkPort : public Port {
  // public:
  //   NetworkPort(Device* device, int id, std::string name, bool digital, bool output) : Port(device, id, name, digital, output) {
  //     this->_value = -1;
  //   }
  // };

  // class VirtualPort : public Port {
  // public:
  //   VirtualPort(Device* device, int id, std::string name, bool digital, bool output) : Port(device, id, name, digital, output) {

  //   }
  // };


  enum DeviceType {
    network_device,
    virtual_device
  };

  class Device {
  public:
    ~Device(void) {
      for (std::map<int, Port*>::iterator it = this->ports.begin(); it != this->ports.end(); ++it) {
        delete(it->second);
      }
    }

    bool set_port_state(int, int);

    bool connect();
    bool is_connected() { return this->connected; }


    boost::property_tree::ptree to_pt();

    const int id;
    const std::string label;
    int ports_count() { return this->ports.size(); }
    Port *port(int id) { return this->ports[id]; }
    std::map<int, Port*> *get_ports() { return &this->ports; }

    // Type
    bool is_network_device() { return this->device_type == network_device; }
    bool is_virtual_device() { return this->device_type == virtual_device; }

    // Serialized fields
    std::string description;
    std::string serial;
    std::string manufacturer_id;
    std::string model;
    boost::signals2::signal<void (std::string)> network_signals;

  protected:
    Device(DeviceType type, int id_, std::string label_, std::string  specs)
      : id(id_), label(label_), device_type(type) {

      this->parse_specifications(specs);
    }
    const DeviceType device_type;
    bool connected;
    void parse_specifications(std::string);
    std::map<int, Port*> ports;

  };

  class NetworkDevice : public Device {
    friend class DeviceConnection;
  public:
    NetworkDevice(int id, const char *label, const char *specs, const char *_password)
      : Device(network_device, id, label, specs), password(_password) {
      this->connected = false;
    }
    static NetworkDevice* find(int);
  private:
    std::string password;
  };


  class VirtualDevice : public Device {
  public:
    VirtualDevice(int id, const char *label, const char *specs)
      : Device(virtual_device, id, label, specs) {
      this->connected = true;
    }
  };


  // Devices List
  void load_devices(void);
  void free_devices(void);
  Device *device_find(int);
  std::map<int, Device*>::iterator devices_iterator(void);
  const std::map<int, Device*> *all_devices();
  bool device_set_port_state(std::string, int);
  std::string devices_to_json();
}

#endif //MODELS_H
