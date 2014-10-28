#include "domoio.h"
#include "jsengine.h"

namespace domoio {
  using namespace v8;
  using namespace std;


  namespace js {

    /**
     * Callbacks
     */
    void file_exists_callback(const v8::FunctionCallbackInfo<Value>& args) {
      if (args.Length() < 1) {
        args.GetIsolate()->ThrowException(v8::String::NewFromUtf8(args.GetIsolate(), "File required"));
        return;
      }

      HandleScope scope(args.GetIsolate());

      std::string filename = object_to_string(args[0]);

      Handle<Boolean> content_handle = v8::Boolean::New(args.GetIsolate(), helpers::file_exists(filename));
      args.GetReturnValue().Set(content_handle);

    }

  }
}
