#include "domoio.h"

namespace domoio {
  // Connection::Connection() {
  // }

  // Register Server Commands
  std::map<std::string, DeviceCommandDef*> device_commands;

  bool register_device_command(std::string name, DeviceCommandDef *def) {
    device_commands[name] = def;
    return true;
  }


  std::map<std::string, ControlCommandDef*> control_commands;
  bool register_control_command(std::string name, ControlCommandDef *def) {
    control_commands[name] = def;
    return true;
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
    DeviceCommandDef *def = device_commands[params[0]];
    if (def == NULL) {
      printf("Invalid Command: '%s'\n", params[0].c_str());
      this->send("400 Bad Request");
      return false;
    }

    def->callback(this, &params);
    return true;
  }

  // Called by dispatch_request
  bool ControlConnection::execute_callback(std::vector<std::string> params) {
    ControlCommandDef *def = control_commands[params[0]];
    if (def == NULL) {
      printf("Invalid Command: '%s'\n", params[0].c_str());
      this->send("400 Bad Request");
      return false;
    }

    def->callback(this, &params);
    return true;
  }

}
