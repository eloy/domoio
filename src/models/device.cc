#include "models.h"
#include "devices.h"
#include "domoio.h"
#include <boost/foreach.hpp>

namespace domoio {

  using namespace vault;
  std::map<int, DeviceState*> DeviceState::active_devices;

  DeviceState::DeviceState(Device *device) : id(device->get_id()), is_virtual(device->is_virtual()) {
  }

  DeviceState *DeviceState::connect(Device *device) {
    DeviceState *state = new DeviceState(device);
    DeviceState::active_devices[device->get_id()] = state;
    return state;
  }

  void DeviceState::disconnect(int device_id) {
    std::map<int,DeviceState *>::iterator it;
    it = DeviceState::active_devices.find(device_id);
    if (it == DeviceState::active_devices.end()) {
      return;
    }

    DeviceState *state = it->second;
    delete(state);
    DeviceState::active_devices.erase(it);
  }


  DeviceState *DeviceState::find(int device_id) {
    std::map<int,DeviceState *>::iterator it;
    it = DeviceState::active_devices.find(device_id);
    if (it == DeviceState::active_devices.end()) {
      return NULL;
    }
    return it->second;
  }


  void DeviceState::load_virtual_devices(void) {
    ModelsCollection<Device> devices;
    Device::all(&devices);

    std::vector<Device*>::iterator it;
    for(it = devices.begin(); it != devices.end(); ++it) {
      Device *device = *it;
      if (device->is_virtual()) {
        DeviceState::active_devices[device->get_id()] = new DeviceState(device);
      }
    }
  }


  void DeviceState::unload_devices(void) {
    for (std::map<int, DeviceState*>::iterator it = DeviceState::active_devices.begin(); it != DeviceState::active_devices.end(); ++it) {
      DeviceState *state = it->second;
      delete(state);

    }
    DeviceState::active_devices.clear();
  }



}
