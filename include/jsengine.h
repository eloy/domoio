#ifndef JSENGINE_H
#define JSENGINE_H


namespace domoio {
  namespace js {
    class JsProcessor;
    typedef boost::shared_ptr<JsProcessor> JsProcessorPtr;


  }

  int init_jsengine();
}

#endif //JSENGINE_H
