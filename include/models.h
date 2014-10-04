#ifndef MODELS_H
#define MODELS_H

#define keypair(type) std::pair<std::string, type>

namespace domoio {

  class Port {
  public:
    Port(std::string name, bool digital, bool output) : _name(name), _digital(digital), _output(output) {};
    const char *name();
    bool digital();
    bool analogic();
    bool output();
    bool input();

  private:
    std::string _name;
    bool _digital;
    bool _output;
  };

  class Device {
  public:
    Device(int, const char*, const char*, const char*);
    ~Device(void);

    bool set_port_state(std::string, std::string);

    bool password_matchs(std::string);

    int id;
    std::string label;
    int ports_count() { return this->ports.size(); }


    // Serialized fields
    std::string description;
    std::string serial;
    std::string manufacturer_id;
    std::string model;
    std::string password;
    boost::signals2::signal<void (std::string)> network_signals;
  private:
    void parse_specifications(const char *);
    std::map<std::string, Port*> ports;

    Port *port(const char* name) { return this->ports[name]; }

  };

  // Devices List
  void load_devices(void);
  void free_devices(void);
  Device *device_find(int);

  bool device_set_port_state(std::string, std::string);
}

#endif //MODELS_H
