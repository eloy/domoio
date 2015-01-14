#include <cstdlib>

#ifndef JSENGINE_H
#define JSENGINE_H
#include <v8.h>

namespace domoio {

  int init_jsengine();

  namespace js {
    std::string object_to_string(v8::Local<v8::Value>);
    // Callbacks
    void devices_callback(const v8::FunctionCallbackInfo<v8::Value>&);
    void log_callback(const v8::FunctionCallbackInfo<v8::Value>&);
    void read_file_callback(const v8::FunctionCallbackInfo<v8::Value>&);
    void file_exists_callback(const v8::FunctionCallbackInfo<v8::Value>&);


    v8::Local<v8::ObjectTemplate> create_device_template(v8::Isolate*);
    v8::Local<v8::ObjectTemplate> create_port_template(v8::Isolate*);

  }
}

#endif //JSENGINE_H
