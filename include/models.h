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


  protected:
    std::string name;
    std::string email;
    std::string encrypted_password;
  };




  class PPort : public vault::Model<PPort> {
  public:
    PPort() : vault::Model<PPort>() {
      this->add_field("name", vault::string, &this->name);
      this->add_field("digital", vault::boolean, &this->digital);
      this->add_field("output", vault::boolean, &this->output);
    }
  protected:
    std::string name;
    bool digital;
    bool output;

  };


  class Specifications : public vault::Model<Specifications> {
  public:
    Specifications() : vault::Model<Specifications>() {
      this->add_field("serial", vault::string, &this->serial);
      this->add_field("manufacturer", vault::string, &this->manufacturer_raw);
      this->add_field("model", vault::string, &this->model_raw);
      this->add_field("description", vault::string, &this->description);
    }

    vault::ModelsCollection<PPort> ports;
    std::string serial;
    std::string manufacturer_raw;
    std::string model_raw;
    std::string description;

  protected:
    virtual void after_from_json_object(json::Object doc) {
      json::Array ports = doc["ports"];
      this->ports.from_json_array(ports);
    }

    virtual void after_to_json_object(json::Object *doc) {
      LOG(error) << this->ports.to_json();
      (*doc)["ports"] = this->ports.to_json_array();
    }
  };




  class Pollo : public vault::Model<Pollo> {
  public:
    static const char* table_name(void) { return "devices"; }
    Pollo() : vault::Model<Pollo>() {
      this->add_field("label", vault::string, &this->label);
      this->add_field("specifications", vault::string, &this->specifications_raw, (vault::DB | vault::FROM_JSON));
      this->add_field("config", vault::string, &this->config_raw, (vault::DB | vault::FROM_JSON));
      this->add_field("type", vault::string, &this->type);
    }

    std::string label;
    std::string specifications_raw;
  protected:
    virtual bool after_load(PGresult *res, int row) {
      this->specifications.from_json(this->specifications_raw);
      return true;
    }

    virtual void after_to_json_object(json::Object *doc) {
      (*doc)["specifications"] = this->specifications.to_json_object();
    }

    std::string config_raw;
    std::string type;
    Specifications specifications;
  };

}

#endif //MODELS_H
