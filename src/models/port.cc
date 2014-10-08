#include "domoio.h"
#include "events.h"

namespace domoio {
  void Port::set_value(int new_value) {
    if (this->value() != new_value) {
      events::send(new Event("port_set", this->device, this, this->value(), new_value));
      this->_value = new_value;
    }
  }

}
