#include "devices.h"
#include "events.h"
#include "domoio_server.h"

namespace domoio {

  void PortState::set_value(bool new_value) {
    int value;
    if (new_value) {
      value = 1;
    } else {
      value = 0;
    }
    this->set_value(value);
  }

  void PortState::set_value(int new_value, bool silent) {

    if (this->value() == new_value) return;

    if (silent == false) {
      events::send(new Event(events::port_set, events::private_channel, this->device, this, this->value(), new_value));

      // Send to device
      if (this->device->is_network_device()) {
        char buffer[CLIENT_BUFFER_MAX_LENGTH];
        snprintf(&buffer[0], CLIENT_BUFFER_MAX_LENGTH, "set %d %d", this->_id, new_value);
        this->send_to_device(&buffer[0]);
      }
    }

    this->_value = new_value;
  }

  bool PortState::send_to_device(const char *data) {
    this->device->network_signals(data);
    return true;
  }

}
