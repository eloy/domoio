#ifndef JSENGINE_H
#define JSENGINE_H


namespace domoio {
  namespace js {
    class JsHttpRequestProcessor;
    typedef boost::shared_ptr<JsHttpRequestProcessor> JsHttpRequestProcessorPtr;


  }

  int init_jsengine();
}

#endif //JSENGINE_H
