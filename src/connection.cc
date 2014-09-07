#include "domoio.h"

namespace domoio {
  // Connection::Connection() {
  // }

  // Register Server Commands
  std::map<std::string, CommandCallback> server_commands;

  int register_server_command(std::string name, CommandCallback command) {
    server_commands[name] = command;
    return 1;
  }


  // Dispatch Request from devices
  void Connection::dispatch_request(std::string str) {
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

    CommandCallback callback = server_commands[params[0]];
    if (callback == NULL) {
      printf("Invalid Command: '%s'\n", params[0].c_str());
      this->send("400 Bad Request");
      return;
    }

    callback(this, &params);
  }

}
