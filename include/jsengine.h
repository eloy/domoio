#include <boost/thread.hpp>
#include <cstdlib>
#ifndef JSENGINE_H
#define JSENGINE_H
#include <v8.h>

namespace domoio {

  class Event;

  using namespace v8;

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

      void queue_event(boost::shared_ptr<Event>);
      bool run();
    private:
      void trigger_event(Event*);

      std::deque< boost::shared_ptr<Event> > queue;
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


  }
}

#endif //JSENGINE_H
