#include "domoio.h"

namespace domoio {
  // Connection::Connection() {
  // }

  // Register Server Commands
  std::map<std::string, CommandDef*> server_commands;

  bool register_server_command(std::string name, CommandDef *def) {

    server_commands[name] = def;
    return true;
  }


  // Dispatch Request from devices
  void Connection::dispatch_request(std::string str) {
    boost::trim_right(str);


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

    CommandDef *def = server_commands[params[0]];
    if (def == NULL) {
      printf("Invalid Command: '%s'\n", params[0].c_str());
      this->send("400 Bad Request");
      return;
    }

    def->callback(this, &params);
  }

}
