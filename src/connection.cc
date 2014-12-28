#include "domoio.h"

namespace domoio {
  // Register Server Commands
  std::map<std::string, DeviceCommandDef*> device_commands;

  bool register_device_command(std::string name, DeviceCommandDef *def) {
    LOG(info) << "Registering device command: '" << name << "'";
    device_commands[name] = def;
    return true;
  }


  std::map<std::string, ControlCommandDef*> control_commands;
  bool register_control_command(std::string name, ControlCommandDef *def) {
    control_commands[name] = def;
    return true;
  }



  namespace commands {
    void create_session(DeviceConnection *, CommandParams);
    void login(DeviceConnection *, CommandParams);
    void all_ports_state(DeviceConnection *, CommandParams);

    void exit(ControlConnection *, CommandParams);
    void get_devices(ControlConnection *, CommandParams);
    void set(ControlConnection *, CommandParams);
    void listen_events(ControlConnection *, CommandParams);

  }

  void init_device_commands() {
    domoio::register_device_command("create_session", new DeviceCommandDef(&commands::create_session, 1, "Start session"));
    domoio::register_device_command("login", new DeviceCommandDef(&commands::login, 1, "Sign in"));
    domoio::register_device_command("aps", new DeviceCommandDef(&commands::all_ports_state, 2, "Update values for all ports"));
  }

  void init_control_commands() {
    domoio::register_control_command("exit", new ControlCommandDef(&commands::exit, 0, "Exit"));
    domoio::register_control_command("get_devices", new ControlCommandDef(&commands::get_devices, 0, "Return Devices"));
    domoio::register_control_command("set", new ControlCommandDef(&commands::set, 2, "Set value at a port"));
    domoio::register_control_command("start_events", new ControlCommandDef(&commands::listen_events, 0, "Start receiving events"));
  }

  // Dispatch Request from devices
  void Connection::dispatch_request(std::string str) {
    boost::trim_right(str);
    LOG(trace) << "Processing request: " << str;

    // Tokenize command
    boost::char_separator<char> separator(" ");
    boost::tokenizer<boost::char_separator<char> > tok(str, separator);
    std::vector<std::string> params;

    for(boost::tokenizer<boost::char_separator<char> >::iterator beg=tok.begin(); beg!=tok.end();++beg){
      params.push_back(*beg);
    }

    if (params.size() == 0) {
      this->send("400 Bad Request");
      return;
    }

    // Execute callback
    this->execute_callback(params);
  }


  // Called by dispatch_request
  bool DeviceConnection::execute_callback(std::vector<std::string> params) {

    std::map<std::string, DeviceCommandDef*>::iterator iter;
    iter = device_commands.find(params[0]);
    if (iter == device_commands.end()) {
      // LOG(info)<< "NOT found: " << iter->first;
      std::map<std::string, DeviceCommandDef*>::iterator it;
      for (it = device_commands.begin(); it != device_commands.end(); ++it) {
        LOG(info) << "Registered: '" << it->first << "'";
      }
      LOG(warning) << "Invalid device command '" << params[0] << "'";
      this->send("400 Bad Request");
      return false;
    }


    DeviceCommandDef *def = iter->second;
    def->callback(this, &params);
    return true;
  }

  // Called by dispatch_request
  bool ControlConnection::execute_callback(std::vector<std::string> params) {
    ControlCommandDef *def = control_commands[params[0]];
    if (def == NULL) {
      LOG(warning) << "Invalid control command: " << params[0];
      this->send("400 Bad Request");
      return false;
    }

    def->callback(this, &params);
    return true;
  }

}
