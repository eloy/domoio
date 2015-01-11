#include "data_mapper.h"
#include <boost/signals2.hpp>
#include "jsengine.h"
#include "json.h"

#ifndef DEVICES_H
#define DEVICES_H

namespace domoio {


  class Device;

  //---------------------------------------------------------------------
  //  STATES
  //---------------------------------------------------------------------


  class DeviceState;

  class PortState {
  public:
    PortState(DeviceState* _device, int id, std::string name, bool digital, bool output)
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
    void set_value(int, bool silent=false);
    void set_value(bool);
    void to_json_object(json::Object&);

  protected:
    int _id;
    std::string _name;
    bool _digital;
    bool _output;
    int _value;
    DeviceState *device;

    bool send_to_device(const char*);
  };

  class DeviceState {
  private:

    DeviceState(Device *);
    ~DeviceState(void) {
      for (std::map<int, PortState*>::iterator it = this->ports.begin(); it != this->ports.end(); ++it) {
        PortState *state = it->second;
        delete(state);
      }
    }

    static std::map<int, DeviceState*> active_devices;


    const bool is_virtual;
    std::map<int, PortState*> ports;


  public:
    static DeviceState *find(int);
    static void load_virtual_devices(void);
    static void unload_devices(void);
    static DeviceState *connect(Device*);
    static void disconnect(int device_id);

    int ports_count() { return this->ports.size(); }
    PortState *port(int id) { return this->ports[id]; }

    // Type
    bool is_network_device() { return !this->is_virtual; }
    bool is_virtual_device() { return this->is_virtual; }

    boost::signals2::signal<void (std::string)> network_signals;
    const int id;
  };

}

#endif //DEVICES_H
