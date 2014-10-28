#ifndef JSENGINE_H
#define JSENGINE_H
#include <v8.h>
#include "helpers.h"

namespace domoio {

  int init_jsengine();

  namespace js {
    std::string object_to_string(v8::Local<v8::Value>);
    // Callbacks
    void log_callback(const v8::FunctionCallbackInfo<v8::Value>&);
    void read_file_callback(const v8::FunctionCallbackInfo<v8::Value>&);
    void file_exists_callback(const v8::FunctionCallbackInfo<v8::Value>&);
  }
}

#endif //JSENGINE_H
