#ifndef MODELS_H
#define MODELS_H

#define keypair(type) std::pair<std::string, type>

namespace domoio {

  class Device;

  class Port {
  public:
  Port(Device* _device, int id, std::string name, bool digital, bool output) : device(_device), _id(id), _name(name), _digital(digital), _output(output) {};
    const char *name();
    bool digital() { return this->_digital; }
    bool analogic() { return !this->_digital; }
    bool output() { return this->_output; }
    bool input() { return !this->_output; }
    int id() { return this->_id; }
    int value() { return this->_value; }
    void set_value(int);
  private:
    int _id;
    std::string _name;
    bool _digital;
    bool _output;
    int _value;
    Device *device;
  };

  class Device {
  public:
    Device(int, const char*, const char*, const char*);
    ~Device(void);

    bool set_port_state(int, int);
    bool password_matchs(std::string);

    bool connect();
    bool is_connected() { return this->connected; }

    int id;
    std::string label;
    int ports_count() { return this->ports.size(); }
    Port *port(int id) { return this->ports[id]; }

    // Serialized fields
    std::string description;
    std::string serial;
    std::string manufacturer_id;
    std::string model;
    std::string password;
    boost::signals2::signal<void (std::string)> network_signals;
  private:
    bool connected;
    void parse_specifications(const char *);
    std::map<int, Port*> ports;

  };

  // Devices List
  void load_devices(void);
  void free_devices(void);
  Device *device_find(int);

  bool device_set_port_state(std::string, int);
}

#endif //MODELS_H
