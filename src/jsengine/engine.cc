#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <boost/thread.hpp>
#include "helpers.h"
#include "config.h"
#include "devices.h"
#include "log.h"
#include "events.h"
#include "jsengine.h"
#include <map>

namespace domoio {
  using namespace v8;
  using namespace std;


  namespace js {

    /**
     * The js engine
     */
    class JsProcessor {
    public:
      // Creates a new processor that processes requests by invoking the
      // Process function of the JavaScript script given as an argument.

      JsProcessor(){ }
      ~JsProcessor();

      bool Initialize();

      void queue_event(EventPtr);
      bool run();
    private:
      void trigger_event(Event*);

      std::deque<EventPtr> queue;
      boost::condition_variable cond;
      boost::mutex mutex;

      Isolate* GetIsolate() { return isolate_; }

      Isolate* isolate_;
      Persistent<Context> context_;


      // Execute the script associated with this processor and extract the
      // Process function.  Returns true if this succeeded, otherwise false.
      bool ExecuteScript(Handle<String> script);


      // Custom
      // Event triggered for sending messages to user coe
      Persistent<Function> trigger_event_process;
    };



    // Convert a JavaScript string to a std::string.  To not bother too
    // much with string encodings we just use ascii.
    string object_to_string(Local<Value> value) {
      String::Utf8Value utf8_value(value);
      return string(*utf8_value);
    }


    // Reads a file into a v8 string.
    Handle<String> read_script(Isolate* isolate, const string &name) {
      std::string content = helpers::read_file(name);
      Handle<String> result = String::NewFromUtf8(isolate, content.c_str(), String::kNormalString, content.length());
      return result;
    }

    // Execute the script and fetch the Process method.
    bool JsProcessor::Initialize() {

      // Create a handle scope to hold the temporary references.
      HandleScope handle_scope(GetIsolate());

      // Create a template for the global object where we set the
      // built-in global functions.
      Handle<ObjectTemplate> global = ObjectTemplate::New(GetIsolate());
      global->Set(String::NewFromUtf8(GetIsolate(), "__log"), FunctionTemplate::New(GetIsolate(), log_callback));
      global->Set(String::NewFromUtf8(GetIsolate(), "__read_file"), FunctionTemplate::New(GetIsolate(), read_file_callback));
      global->Set(String::NewFromUtf8(GetIsolate(), "__file_exists"), FunctionTemplate::New(GetIsolate(), file_exists_callback));
      global->Set(String::NewFromUtf8(GetIsolate(), "__devices"), FunctionTemplate::New(GetIsolate(), devices_callback));


      // Each processor gets its own context so different processors don't
      // affect each other. Context::New returns a persistent handle which
      // is what we need for the reference to remain after we return from
      // this method. That persistent handle has to be disposed in the
      // destructor.
      v8::Handle<v8::Context> context = Context::New(GetIsolate(), NULL, global);
      context_.Reset(GetIsolate(), context);

      // Enter the new context so all the following operations take place
      // within it.
      Context::Scope context_scope(context);


      // Run system script
      context->Global()->Set(String::NewFromUtf8(GetIsolate(), "__DOMOIO_CORE_ROOT"), String::NewFromUtf8(GetIsolate(), conf_opt::js_system_path.c_str()));
      context->Global()->Set(String::NewFromUtf8(GetIsolate(), "__DOMOIO_USER_ROOT"), String::NewFromUtf8(GetIsolate(), conf_opt::js_user_path.c_str()));
      std::string system_application_script_path = conf_opt::js_system_path + "/application.js";
      Handle<String> script = read_script(GetIsolate(), system_application_script_path);

      if (script.IsEmpty()) {
        LOG(fatal) << "Error reading " << system_application_script_path;
        return false;
      }

      // Compile and run the script
      if (!ExecuteScript(script))
        return false;

      // The script compiled and ran correctly.  Now we fetch out the
      // Process function from the global object.
      Handle<String> process_name = String::NewFromUtf8(GetIsolate(), "__emit_events");
      Handle<Value> process_val = context->Global()->Get(process_name);

      if (!process_val->IsFunction()) return false;

      Handle<Function> process_fun = Handle<Function>::Cast(process_val);

      // // Store the function in a Persistent handle, since we also want
      // // that to remain after this call returns
      trigger_event_process.Reset(GetIsolate(), process_fun);

      // All done; all went well
      return true;
    }


    bool JsProcessor::ExecuteScript(Handle<String> script) {
      HandleScope handle_scope(GetIsolate());

      // We're just about to compile the script; set up an error handler to
      // catch any exceptions the script might throw.
      TryCatch try_catch;
      // Compile the script and check for errors.
      Handle<Script> compiled_script = Script::Compile(script);
      if (compiled_script.IsEmpty()) {
        String::Utf8Value error(try_catch.Exception());
        LOG(fatal) << *error;
        return false;
      }

      // Run the script!
      Handle<Value> result = compiled_script->Run();
      if (result.IsEmpty()) {
        // The TryCatch above is still in effect and will have caught the error.
        String::Utf8Value stack_trace(try_catch.StackTrace());
        LOG(fatal) << *stack_trace;
        return false;
      }
      return true;
    }



    JsProcessor::~JsProcessor() {
      // Dispose the persistent handles.  When noone else has any
      // references to the objects stored in the handles they will be
      // automatically reclaimed.
      context_.Reset();
      trigger_event_process.Reset();
    }


    void JsProcessor::queue_event(EventPtr event) {
      this->queue.push_back(event);
      boost::lock_guard<boost::mutex> lock(this->mutex);
      this->cond.notify_one();
    }


    bool JsProcessor::run() {
      V8::InitializeICU();
      this->isolate_ = Isolate::New();

      v8::Locker locker(this->isolate_);
      Isolate::Scope isolate_scope(this->isolate_);
      HandleScope scope(this->isolate_);

      if (!this->Initialize()) {
        LOG(fatal) << "Error initializing JS engine";
        return false;
      }
      v8::Unlocker unlocker(GetIsolate());

      // Subscribe to events
      signals_connection events_connection = domoio::events::add_listener(boost::bind(&JsProcessor::queue_event, this, _1));

      // Wait for events
      boost::unique_lock<boost::mutex> lock(this->mutex);
      while(true) {

        while(this->queue.empty()) {
          this->cond.wait(lock);
        }

        EventPtr event_ptr = this->queue.front();
        this->queue.pop_front();
        Event *event = event_ptr.get();
        this->trigger_event(event);
      }

      events_connection.disconnect();
      return true;
    }

    void JsProcessor::trigger_event(Event *event) {
      LOG(trace) << "Ready to execute JS thread: " << boost::this_thread::get_id();
      v8::Locker locker(GetIsolate());
      HandleScope handle_scope(GetIsolate());


      v8::Local<v8::Context> context = v8::Local<v8::Context>::New(GetIsolate(), context_);
      // Context::Scope context_scope(context);

      TryCatch try_catch;
      v8::Local<v8::Function> process = v8::Local<v8::Function>::New(GetIsolate(), trigger_event_process);

      Handle<Value> argv[2] = {String::NewFromUtf8(GetIsolate(), event->channel_name().c_str()), String::NewFromUtf8(GetIsolate(), event->json.c_str())};
      Handle<Value> result = process->Call(context->Global(), 2, argv);

      if (result.IsEmpty()) {
        // The TryCatch above is still in effect and will have caught the error.
        String::Utf8Value error(try_catch.Exception());
        // Log(*error);
        std::cout << *error;
        // Running the script failed; bail out.
      }
      v8::Unlocker unlocker(GetIsolate());
    }


    void run_engine() {
      JsProcessor processor;
      processor.run();
    }

  }


  // START THE ENGINE
  int init_jsengine() {
    boost::thread thread(js::run_engine);
    return 1;
  }

}
