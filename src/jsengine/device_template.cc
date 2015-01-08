#include "devices.h"
#include "jsengine.h"

namespace domoio {
  using namespace v8;
  using namespace std;


  namespace js {

    // ID
    void get_id(Local<String> property, const PropertyCallbackInfo<v8::Value> &info) {
      Local<Object> self = info.Holder();
      Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
      void* ptr = wrap->Value();
      int value = static_cast<Device*>(ptr)->id;
      info.GetReturnValue().Set(value);
    }


    // Label
    void get_label(Local<String> property, const PropertyCallbackInfo<v8::Value> &info) {
      Local<Object> self = info.Holder();
      Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
      void* ptr = wrap->Value();
      std::string value = static_cast<Device*>(ptr)->label;
      info.GetReturnValue().Set(v8::String::NewFromUtf8(info.GetIsolate(), value.c_str()));
    }


    void set_label(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info) {
      // Local<Object> self = info.Holder();
      // Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
      // void* ptr = wrap->Value();
      // static_cast<Device*>(ptr)->label.assign(object_to_string(value));
    }

    // Ports
    void get_ports(Local<String> property, const PropertyCallbackInfo<v8::Value> &info) {
      Local<Object> self = info.Holder();
      Local<External> wrap = Local<External>::Cast(self->GetInternalField(0));
      void* ptr = wrap->Value();
      std::map<int, Port*> *ports_map = static_cast<Device*>(ptr)->get_ports();


      v8::Isolate* isolate = info.GetIsolate();

      EscapableHandleScope handle_scope(isolate);
      Local<Array> array = Array::New(isolate, ports_map->size());

      // Return an empty result if there was an error creating the array.
      if (array.IsEmpty()) {
        info.GetReturnValue().Set(Local<Array>());
        return;
      }


      Local<ObjectTemplate> port_templ = create_port_template(isolate);

      int index = 0;
      for (std::map<int, Port*>::iterator it = ports_map->begin(); it != ports_map->end(); ++it) {
        Port *port = it->second;
        Local<Object> obj = port_templ->NewInstance();
        obj->SetInternalField(0, External::New(isolate, port));
        array->Set(index++, obj);
      }
      info.GetReturnValue().Set(handle_scope.Escape(array));
    }


    Local<ObjectTemplate> create_device_template(v8::Isolate *isolate) {
      Local<ObjectTemplate> device_templ = ObjectTemplate::New(isolate);
      device_templ->SetInternalFieldCount(1);
      device_templ->SetAccessor(String::NewFromUtf8(isolate, "id"), get_id);
      device_templ->SetAccessor(String::NewFromUtf8(isolate, "ports"), get_ports);
      device_templ->SetAccessor(String::NewFromUtf8(isolate, "label"), get_label, set_label);
      return device_templ;
    }
  }
}
