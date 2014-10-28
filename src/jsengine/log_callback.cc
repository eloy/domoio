#include "domoio.h"
#include "jsengine.h"

namespace domoio {
  using namespace v8;
  using namespace std;


  namespace js {

    /**
     * Callbacks
     */
    void log_callback(const v8::FunctionCallbackInfo<Value>& args) {
      if (args.Length() < 1) return;
      HandleScope scope(args.GetIsolate());
      Handle<Value> arg = args[0];
      String::Utf8Value value(arg);
      LOG(info) << "JS: " << *value << "\n";
    }

  }
}
