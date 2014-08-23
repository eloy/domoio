#ifndef SUPPORT_H
#define SUPPORT_H

namespace domoio {
  void debug_command_params(CommandParams params) {
    for (int i=0; i < params->size(); i++) {
      printf("%d => %s\n", i, params->at(i).c_str());
    }
  }
}

#endif //SUPPORT_H
