#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include <exception>

namespace domoio {
  class DomoioException: public std::exception {
  public:
    DomoioException(const char *_message) : message(_message) {};
    virtual const char* what() const throw() {
      return message;
    }
  private:
    const char *message;
  };

}

#endif //EXCEPTIONS_H
