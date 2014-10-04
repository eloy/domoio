#include "domoio.h"

namespace domoio {

  const char *Port::name(void) {
    return _name.c_str();
  }

  bool Port::digital(void) {
    return _digital;
  }

  bool Port::analogic(void) {
    return !_digital;
  }

  bool Port::input(void) {
    return !_output;
  }

  bool Port::output(void) {
    return _output;
  }

}
