#include "domoio.h"

namespace domoio {
  void Port::set_value(int new_value) {
    if (this->value() != new_value) {
      events::send(new Event(events::port_set, events::private_channel, this->device, this, this->value(), new_value));
      this->_value = new_value;
    }
  }

  // boost::property_tree::ptree Port::to_pt() {

  //   boost::property_tree::ptree pt;

  //   pt.put("id", this->_id);
  //   pt.put("name", this->_name);
  //   pt.put("digital", this->digital());
  //   pt.put("output", this->output());

  //   if (this->digital()) {
  //     if (this->value() > 0) {
  //       pt.put("value", true);
  //     } else {
  //       pt.put("value", false);
  //     }
  //   } else {
  //     pt.put("value", this->value());
  //   }

  //   return pt;
  // }

  void Port::to_json_object(json::Object &d) {
    d["id"] = json::Number(this->_id);
    d["name"] = json::String(this->_name);
    d["digital"] = json::Boolean(this->digital());
    d["output"] = json::Boolean(this->output());
    if (this->digital()) {
      if (this->value() > 0) {
        d["value"] = json::Boolean(true);
      } else {
        d["value"] = json::Boolean(false);
      }
    } else {
      d["value"] = json::Number(this->value());
    }
  }
}
