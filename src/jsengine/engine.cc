#include <v8.h>
#include "domoio.h"
#include "jsengine.h"
#include <map>

namespace domoio {
  using namespace v8;
  using namespace std;


  namespace js {

    /**
     * An http request processor that is scriptable using JavaScript.
     */
    class JsProcessor {
    public:
      // Creates a new processor that processes requests by invoking the
      // Process function of the JavaScript script given as an argument.
      JsProcessor(Isolate* isolate, Handle<String> script)
        : isolate_(isolate), script_(script) { }
      virtual ~JsProcessor();

      virtual bool Initialize(map<string, string>* opts,
                              map<string, string>* output);
      // virtual bool Process(HttpRequest* req);

      virtual void send_event(EventPtr);
    private:

      Isolate* GetIsolate() { return isolate_; }

      Isolate* isolate_;
      Handle<String> script_;
      Persistent<Context> context_;
      // Persistent<Function> process_;
      static Persistent<ObjectTemplate> request_template_;
      static Persistent<ObjectTemplate> map_template_;


      // Execute the script associated with this processor and extract the
      // Process function.  Returns true if this succeeded, otherwise false.
      bool ExecuteScript(Handle<String> script);



      // Constructs the template that describes the JavaScript wrapper
      // type for requests.
      static Handle<ObjectTemplate> MakeMapTemplate(Isolate* isolate);


      // Wrap the options and output map in a JavaScript objects and
      // install it in the global namespace as 'options' and 'output'.
      bool InstallMaps(map<string, string>* opts, map<string, string>* output);
      // Utility methods for wrapping C++ objects as JavaScript objects,
      // and going back again.
      Handle<Object> WrapMap(map<string, string>* obj);
      static map<string, string>* UnwrapMap(Handle<Object> obj);

      // Callbacks that access maps
      static void MapGet(Local<String> name, const PropertyCallbackInfo<Value>& info);

      static void MapSet(Local<String> name, Local<Value> value, const PropertyCallbackInfo<Value>& info);




    };



    //----------------------------------------------------------------



    Persistent<Function> process_;



    // Convert a JavaScript string to a std::string.  To not bother too
    // much with string encodings we just use ascii.
    string ObjectToString(Local<Value> value) {
      String::Utf8Value utf8_value(value);
      return string(*utf8_value);
    }


    /**
     * Callbacks
     */
    static void LogCallback(const v8::FunctionCallbackInfo<v8::Value>& args) {
      if (args.Length() < 1) return;
      HandleScope scope(args.GetIsolate());
      Handle<Value> arg = args[0];
      String::Utf8Value value(arg);
      LOG(info) << "JSENGINE: " << *value << "\n";
    }


    static void add_event(const v8::FunctionCallbackInfo<v8::Value>& args) {
      if (args.Length() < 2) return;
      HandleScope scope(args.GetIsolate());
      String::Utf8Value device_id(args[0]);
      Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(args[1]);
      // Store the function in a Persistent handle, since we also want
      // that to remain after this call returns
      process_.Reset(args.GetIsolate(), func);

      LOG(trace) << "Device id: " << *device_id << "\n";
    }



    // Execute the script and fetch the Process method.
    bool JsProcessor::Initialize(map<string, string>* opts, map<string, string>* output) {

      // Create a handle scope to hold the temporary references.
      HandleScope handle_scope(GetIsolate());

      // Create a template for the global object where we set the
      // built-in global functions.
      Handle<ObjectTemplate> global = ObjectTemplate::New(GetIsolate());
      global->Set(String::NewFromUtf8(GetIsolate(), "log"), FunctionTemplate::New(GetIsolate(), LogCallback));

      global->Set(String::NewFromUtf8(GetIsolate(), "addEvent"), FunctionTemplate::New(GetIsolate(), add_event));


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

      // Make the options mapping available within the context
      if (!InstallMaps(opts, output))
        return false;

      // Compile and run the script
      if (!ExecuteScript(script_))
        return false;

      // The script compiled and ran correctly.  Now we fetch out the
      // Process function from the global object.
      // Handle<String> process_name = String::NewFromUtf8(GetIsolate(), "Process");
      // Handle<Value> process_val = context->Global()->Get(process_name);

      // // If there is no Process function, or if it is not a function,
      // // bail out
      // if (!process_val->IsFunction()) return false;

      // // It is a function; cast it to a Function
      // Handle<Function> process_fun = Handle<Function>::Cast(process_val);

      // // Store the function in a Persistent handle, since we also want
      // // that to remain after this call returns
      // process_.Reset(GetIsolate(), process_fun);

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
        // Log(*error);
        std::cout << *error;
        // The script failed to compile; bail out.
        return false;
      }

      // Run the script!
      Handle<Value> result = compiled_script->Run();
      if (result.IsEmpty()) {
        // The TryCatch above is still in effect and will have caught the error.
        String::Utf8Value error(try_catch.Exception());
        // Log(*error);
        std::cout << *error;
        // Running the script failed; bail out.
        return false;
      }
      return true;
    }



    // -----------------------------------
    // --- A c c e s s i n g   M a p s ---
    // -----------------------------------

    // Utility function that wraps a C++ http request object in a
    // JavaScript object.
    Handle<Object> JsProcessor::WrapMap(map<string, string>* obj) {
      // Handle scope for temporary handles.
      EscapableHandleScope handle_scope(GetIsolate());

      // Fetch the template for creating JavaScript map wrappers.
      // It only has to be created once, which we do on demand.
      if (map_template_.IsEmpty()) {
        Handle<ObjectTemplate> raw_template = MakeMapTemplate(GetIsolate());
        map_template_.Reset(GetIsolate(), raw_template);
      }
      Handle<ObjectTemplate> templ =
        Local<ObjectTemplate>::New(GetIsolate(), map_template_);

      // Create an empty map wrapper.
      Local<Object> result = templ->NewInstance();

      // Wrap the raw C++ pointer in an External so it can be referenced
      // from within JavaScript.
      Handle<External> map_ptr = External::New(GetIsolate(), obj);

      // Store the map pointer in the JavaScript wrapper.
      result->SetInternalField(0, map_ptr);

      // Return the result through the current handle scope.  Since each
      // of these handles will go away when the handle scope is deleted
      // we need to call Close to let one, the result, escape into the
      // outer handle scope.
      return handle_scope.Escape(result);
    }

    // Utility function that extracts the C++ map pointer from a wrapper
    // object.
    map<string, string>* JsProcessor::UnwrapMap(Handle<Object> obj) {
      Handle<External> field = Handle<External>::Cast(obj->GetInternalField(0));
      void* ptr = field->Value();
      return static_cast<map<string, string>*>(ptr);
    }


    Handle<ObjectTemplate> JsProcessor::MakeMapTemplate(Isolate* isolate) {

      EscapableHandleScope handle_scope(isolate);

      Local<ObjectTemplate> result = ObjectTemplate::New(isolate);
      result->SetInternalFieldCount(1);
      result->SetNamedPropertyHandler(MapGet, MapSet);

      // Again, return the result through the current handle scope.
      return handle_scope.Escape(result);
    }



    bool JsProcessor::InstallMaps(map<string, string>* opts,map<string, string>* output) {

      HandleScope handle_scope(GetIsolate());

      // Wrap the map object in a JavaScript wrapper
      Handle<Object> opts_obj = WrapMap(opts);

      v8::Local<v8::Context> context = v8::Local<v8::Context>::New(GetIsolate(), context_);

      // Set the options object as a property on the global object.
      context->Global()->Set(String::NewFromUtf8(GetIsolate(), "options"), opts_obj);


      Handle<Object> output_obj = WrapMap(output);
      context->Global()->Set(String::NewFromUtf8(GetIsolate(), "output"), output_obj);


      return true;
    }


    void JsProcessor::MapGet(Local<String> name, const PropertyCallbackInfo<Value>& info) {

      // Fetch the map wrapped by this object.
      map<string, string>* obj = UnwrapMap(info.Holder());

      // Convert the JavaScript string to a std::string.
      string key = ObjectToString(name);

      // Look up the value if it exists using the standard STL ideom.
      map<string, string>::iterator iter = obj->find(key);

      // If the key is not present return an empty handle as signal
      if (iter == obj->end()) return;

      // Otherwise fetch the value and wrap it in a JavaScript string
      const string& value = (*iter).second;
      info.GetReturnValue().Set(String::NewFromUtf8( info.GetIsolate(), value.c_str(), String::kNormalString, static_cast<int>(value.length()) ));


    }

    void JsProcessor::MapSet(Local<String> name, Local<Value> value_obj, const PropertyCallbackInfo<Value>& info) {


      // Fetch the map wrapped by this object.
      map<string, string>* obj = UnwrapMap(info.Holder());

      // Convert the key and value to std::strings.
      string key = ObjectToString(name);
      string value = ObjectToString(value_obj);

      // Update the map.
      (*obj)[key] = value;

      // Return the value; any non-empty handle will work.
      info.GetReturnValue().Set(value_obj);
    }






    JsProcessor::~JsProcessor() {
      // Dispose the persistent handles.  When noone else has any
      // references to the objects stored in the handles they will be
      // automatically reclaimed.
      context_.Reset();
      process_.Reset();
    }


    // Persistent<ObjectTemplate> JsProcessor::request_template_;
    Persistent<ObjectTemplate> JsProcessor::map_template_;




    // Reads a file into a v8 string.
    Handle<String> ReadFile(Isolate* isolate, const string& name) {
      FILE* file = fopen(name.c_str(), "rb");
      if (file == NULL) return Handle<String>();

      fseek(file, 0, SEEK_END);
      int size = ftell(file);
      rewind(file);

      char* chars = new char[size + 1];
      chars[size] = '\0';
      for (int i = 0; i < size;) {
        int read = static_cast<int>(fread(&chars[i], 1, size - i, file));
        i += read;
      }
      fclose(file);
      Handle<String> result =
        String::NewFromUtf8(isolate, chars, String::kNormalString, size);
      delete[] chars;
      return result;
    }


    //
    // void run_callback(Isolate *isolate) {

    //   // Create a handle scope to hold the temporary references.
    //   // HandleScope handle_scope(isolate);

    //   // Create a template for the global object where we set the
    //   // built-in global functions.
    //   Handle<ObjectTemplate> global = ObjectTemplate::New(isolate);
    //   global->Set(String::NewFromUtf8(isolate, "log"), FunctionTemplate::New(isolate, LogCallback));

    //   global->Set(String::NewFromUtf8(isolate, "addEvent"), FunctionTemplate::New(isolate, add_event));


    //   // Each processor gets its own context so different processors don't
    //   // affect each other. Context::New returns a persistent handle which
    //   // is what we need for the reference to remain after we return from
    //   // this method. That persistent handle has to be disposed in the
    //   // destructor.
    //   v8::Handle<v8::Context> context = Context::New(isolate, NULL, global);

    //   // Enter the new context so all the following operations take place
    //   // within it.
    //   Context::Scope context_scope(context);

    //   v8::Local<v8::Function> process = v8::Local<v8::Function>::New(isolate, process_);

    //   Handle<Value> argv[2] = {String::NewFromUtf8(isolate, "event_type"), String::NewFromUtf8(isolate, "event_data")};
    //   Handle<Value> result = process->Call(context->Global(), 2, argv);

    // }




    void JsProcessor::send_event(EventPtr event_ptr) {
      Event *event = event_ptr.get();
      LOG(trace) << "Ready to execute JS thread: " << boost::this_thread::get_id();
      v8::Locker locker(GetIsolate());
      HandleScope handle_scope(GetIsolate());

      // We're just about to compile the script; set up an error handler to
      // catch any exceptions the script might throw.


      v8::Local<v8::Context> context = v8::Local<v8::Context>::New(GetIsolate(), context_);
      // Context::Scope context_scope(context);

      TryCatch try_catch;
      v8::Local<v8::Function> process = v8::Local<v8::Function>::New(GetIsolate(), process_);

      Handle<Value> argv[2] = {String::NewFromUtf8(GetIsolate(), event->channel_name().c_str()), String::NewFromUtf8(GetIsolate(), event->to_json().c_str())};
      // Handle<Value> argv[2] = {String::NewFromUtf8(GetIsolate(), event->channel_name().c_str()), String::NewFromUtf8(GetIsolate(), "pollo")};
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




  // Service




    class ScriptService : public boost::asio::io_service::service {
    public:

      /// The unique service identifier.
      static boost::asio::io_service::id id;

      ScriptService(boost::asio::io_service&);
      ~ScriptService() {}
      void shutdown_service() {}
      void send(EventPtr);

      void handle(EventPtr);

      void init_jsengine();

      // Events
      signals_connection events_connection;
      JsProcessor *processor;
    };


    boost::asio::io_service::id ScriptService::id;
    boost::asio::io_service io_service;
    ScriptService events_service(io_service);
    boost::asio::io_service::work work(io_service);


    ScriptService::ScriptService(boost::asio::io_service& io_service) : boost::asio::io_service::service(io_service) {
    }

    // void ScriptService::init_jsengine() {
    //   LOG(trace) << "Init jsengine thread: " << boost::this_thread::get_id();
    //   V8::InitializeICU();
    //   isolate = Isolate::New();
    //   // v8::Locker locker(isolate);
    //   Isolate::Scope isolate_scope(isolate);
    //   HandleScope scope(isolate);

    //   string file("test.js");
    //   Handle<String> source = ReadFile(isolate, file);
    //   if (source.IsEmpty()) {
    //     fprintf(stderr, "Error reading '%s'.\n", file.c_str());
    //     return;
    //   }
    //   this->processor = new JsProcessor(isolate, source);


    //   if (!processor->Initialize(&options, &output)) {
    //     fprintf(stderr, "Error initializing processor.\n");
    //     return;
    //   }

    //   this->events_connection = domoio::events::add_listener(boost::bind(&ScriptService::send, this, _1));
    // }



    void ScriptService::send(EventPtr event_ptr) {
      io_service.post(boost::bind(&ScriptService::handle, this, event_ptr));
    }

    void ScriptService::handle(EventPtr event_ptr) {
      processor->send_event(event_ptr);
    }


    // Start the js engine thread
    static void run_io_service() {
      LOG(trace) << "Starting JS engine";


      V8::InitializeICU();
      v8::Isolate* isolate = Isolate::New();

      // v8::Locker locker(isolate);
      Isolate::Scope isolate_scope(isolate);
      HandleScope scope(isolate);

      string file("test.js");
      Handle<String> source = ReadFile(isolate, file);
      if (source.IsEmpty()) {
        fprintf(stderr, "Error reading '%s'.\n", file.c_str());
        return;
      }
      events_service.processor = new JsProcessor(isolate, source);

      map<string, string> options;
      map<string, string> output;

      if (!events_service.processor->Initialize(&options, &output)) {
        fprintf(stderr, "Error initializing processor.\n");
        return;
      }

      events_service.events_connection = domoio::events::add_listener(boost::bind(&ScriptService::send, &events_service, _1));

      io_service.run();
    }

    bool started = false;
    bool start(void) {

      try {
        if (!started) {
          started = true;
        }
        else {
          io_service.reset();
        }

        boost::thread thread(run_io_service);
        return true;
      }
      catch (std::exception& e) {
        LOG(fatal) << "Exception: " << e.what() << "\n";
        return false;
      }
    }


    bool stop(void) {
      io_service.stop();
      // m_threads.join_all();
      io_service.reset();
      return true;
    }

  }


  // START THE ENGINE
  int init_jsengine() {
    return js::start();
  }

}
