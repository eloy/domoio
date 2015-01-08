#include "domoio_server.h"
#include <boost/algorithm/string/trim.hpp>

namespace domoio {
  // Register Server Commands
  std::map<std::string, DeviceCommandDef*> device_commands;

  bool register_device_command(std::string name, DeviceCommandDef *def) {
    LOG(trace) << "Registering device command: '" << name << "'";
    device_commands[name] = def;
    return true;
  }


  namespace commands {
    void create_session(DeviceConnection *, CommandParams);
    void login(DeviceConnection *, CommandParams);
    void all_ports_state(DeviceConnection *, CommandParams);
    void exit(DeviceConnection *, CommandParams);
    void set(DeviceConnection *, CommandParams);
  }

  void register_device_commands() {
    domoio::register_device_command("create_session", new DeviceCommandDef(&commands::create_session, 1, "Start session"));
    domoio::register_device_command("login", new DeviceCommandDef(&commands::login, 1, "Sign in"));
    domoio::register_device_command("aps", new DeviceCommandDef(&commands::all_ports_state, 2, "Update values for all ports"));
    domoio::register_device_command("exit", new DeviceCommandDef(&commands::exit, 0, "Exit"));
    domoio::register_device_command("set", new DeviceCommandDef(&commands::set, 2, "Set value at a port"));
  }

  void unregister_device_commands() {
    std::map<std::string, DeviceCommandDef*>::iterator it;
    for (it = device_commands.begin(); it != device_commands.end(); ++it) {
      DeviceCommandDef* command = it->second;
      delete(command);
    }

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
      LOG(warning) << "Invalid device command '" << params[0] << "'";
      this->send("400 Bad Request");
      return false;
    }


    DeviceCommandDef *def = iter->second;
    def->callback(this, &params);
    return true;
  }

}
