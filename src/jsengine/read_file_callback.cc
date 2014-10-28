#include "domoio.h"
#include "jsengine.h"

namespace domoio {
  using namespace v8;
  using namespace std;


  namespace js {

    /**
     * Callbacks
     */
    void read_file_callback(const v8::FunctionCallbackInfo<Value>& args) {
      if (args.Length() < 1) {
        args.GetIsolate()->ThrowException(v8::String::NewFromUtf8(args.GetIsolate(), "File required"));
        return;
      }

      HandleScope scope(args.GetIsolate());

      std::string filename = object_to_string(args[0]);

      if(helpers::file_exists(filename) == false) {
        args.GetIsolate()->ThrowException(v8::String::NewFromUtf8(args.GetIsolate(), "File not found"));
        return;
      }

      std::string content = helpers::read_file(filename);
      Handle<String> content_handle = String::NewFromUtf8(args.GetIsolate(), content.c_str(), String::kNormalString, content.length());
      args.GetReturnValue().Set(content_handle);

    }

  }
}
