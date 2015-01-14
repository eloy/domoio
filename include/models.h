#include "data_mapper.h"
#ifndef MODELS_H
#define MODELS_H

namespace domoio {
  class User : public vault::Model<User> {
  public:
    static const char* table_name(void) { return "users"; }
    User() : vault::Model<User>() {
      this->add_field("name", vault::string, &this->name);
      this->add_field("email", vault::string, &this->email);
      this->add_field("encrypted_password", vault::string, &this->encrypted_password, vault::DB);
    }


    std::string get_name() { return this->name;}
    void set_name(std::string new_name) { this->name.assign(new_name); }
    std::string get_email() { return this->email;}
    void set_email(std::string new_email) { this->email.assign(new_email); }

    virtual User* clone() const { return new User(*this); }

  protected:
    std::string name;
    std::string email;
    std::string encrypted_password;
  };



  class Device;

  class Port : public vault::Model<Port> {
  public:
  Port() : vault::Model<Port>() {
      this->add_field("id", vault::integer, &this->id, vault::FROM_JSON);
      this->add_field("name", vault::string, &this->name);
      this->add_field("digital", vault::boolean, &this->digital);
      this->add_field("output", vault::boolean, &this->output);
    }
    std::string name;
    bool digital;
    bool output;
    void set_id(int new_id) { this->id = new_id;}
    // Link to the device.
    // we setup this in device_template.cc, method get_ports
    Device *device;
  private:
    virtual void after_from_json_object(json::Object *doc);
    virtual void after_to_json_object(json::Object *doc);
  };


  class Specifications : public vault::Model<Specifications> {
    friend class DeviceState;
    friend class Device;
  public:
  Specifications() : vault::Model<Specifications>() {
      this->add_field("serial", vault::string, &this->serial);
      this->add_field("manufacturer", vault::string, &this->manufacturer_raw);
      this->add_field("model", vault::string, &this->model_raw);
      this->add_field("description", vault::string, &this->description);
    }

    ~Specifications();

    std::string serial;
    std::string manufacturer_raw;
    std::string model_raw;
    std::string description;

  protected:
    std::map<int, Port*> ports;
    virtual void after_from_json_object(json::Object *doc);
    virtual void after_to_json_object(json::Object *doc);
  };




  class Device : public vault::Model<Device> {
    friend class DeviceState;
  public:
    static const char* table_name(void) { return "devices"; }
  Device() : vault::Model<Device>() {
      this->add_field("label", vault::string, &this->label);
      this->add_field("password", vault::string, &this->password, (vault::DB | vault::FROM_JSON));
      this->add_field("specifications", vault::text, &this->specifications_raw, vault::DB);
      this->add_field("config", vault::text, &this->config_raw, (vault::DB | vault::FROM_JSON));
      this->add_field("type", vault::string, &this->type);
    }

    Specifications *get_specifications() { return &this->specifications; }
    std::map<int, Port*> *get_ports() { return &this->specifications.ports; }

    Port * get_port(int);
    void add_port(Port *);

    std::string label;
    std::string password;
    std::string specifications_raw;

    bool is_virtual() { return this->type == "VirtualDevice"; }

  protected:
    virtual bool after_load(PGresult* , int);
    virtual bool before_save();
    virtual void after_from_json_object(json::Object*);

    virtual void after_to_json_object(json::Object*);

    std::string config_raw;
    std::string type;
    Specifications specifications;
  };



}

#endif //MODELS_H
