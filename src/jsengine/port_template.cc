#include "models.h"
#include "devices.h"
#include "jsengine.h"

namespace domoio {
  using namespace v8;
  using namespace std;


  namespace js {

    // ID
    void get_port_id(Local<String> property, const PropertyCallbackInfo<v8::Value> &info) {
      Local<Object> self = info.Holder();
      Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
      void* ptr = wrap->Value();
      int value = static_cast<Port*>(ptr)->get_id();
      info.GetReturnValue().Set(value);
    }

    // Name
    void get_port_name(Local<String> property, const PropertyCallbackInfo<v8::Value> &info) {
      v8::Isolate* isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
      void* ptr = wrap->Value();
      std::string value = static_cast<Port*>(ptr)->name;
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, value.c_str()));
    }


    // Value
    void get_port_value(Local<String> property, const PropertyCallbackInfo<v8::Value> &info) {
      Local<Object> self = info.Holder();
      Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
      void* ptr = wrap->Value();
      int value = static_cast<Port*>(ptr)->get_id();
      bool digital = static_cast<Port*>(ptr)->digital;
      if (digital) {
        if (value > 0) {
          info.GetReturnValue().Set(true);
        }else {
          info.GetReturnValue().Set(false);
        }
      } else {
        info.GetReturnValue().Set(value);
      }
    }


    void set_port_value(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info) {
      Local<Object> self = info.Holder();
      Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
      void* ptr = wrap->Value();
      int value_int;
      Port *port = static_cast<Port*>(ptr);
      // TODO: Check port is input
      if (value->IsBoolean()) {
        // TODO: Check if port is digital
        if (value->BooleanValue()) {
          value_int = 1;
        } else {
          value_int = 0;
        }
      }
      else if (value->IsInt32()) {
        // TODO: Check if port is analogic
        value_int = value->Int32Value();
      }

      // TODO: Implement
      // port->set_value(value_int);
    }

    Local<ObjectTemplate> create_port_template(v8::Isolate *isolate) {
      Local<ObjectTemplate> port_templ = ObjectTemplate::New(isolate);
      port_templ->SetInternalFieldCount(1);
      port_templ->SetAccessor(String::NewFromUtf8(isolate, "id"), get_port_id);
      port_templ->SetAccessor(String::NewFromUtf8(isolate, "label"), get_port_name);
      port_templ->SetAccessor(String::NewFromUtf8(isolate, "value"), get_port_value, set_port_value);
      return port_templ;
    }
  }
}
