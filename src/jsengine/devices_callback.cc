#include "domoio.h"
#include "jsengine.h"

namespace domoio {
  using namespace v8;
  using namespace std;


  namespace js {

    /**
     * Callbacks
     */
    void devices_callback(const v8::FunctionCallbackInfo<Value>& args) {
      v8::Isolate* isolate = args.GetIsolate();
      const std::map<int, Device*> *devices_map = all_devices();

      EscapableHandleScope handle_scope(isolate);
      Local<Array> array = Array::New(isolate, devices_map->size());

      // Return an empty result if there was an error creating the array.
      if (array.IsEmpty()) {
        args.GetReturnValue().Set(Local<Array>());
        return;
      }


      Local<ObjectTemplate> device_templ = create_device_template(isolate);

      int index = 0;
      for (std::map<int, Device*>::iterator it = devices_iterator(); it != devices_map->end(); ++it) {
        Device *d = it->second;
        Local<Object> obj = device_templ->NewInstance();
        obj->SetInternalField(0, External::New(isolate, d));
        array->Set(index++, obj);
      }
      args.GetReturnValue().Set(handle_scope.Escape(array));
    }

  }
}
