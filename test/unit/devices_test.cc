#include "models.h"
#include "domoio_test.h"

TEST(DeviceState, find) {
  domoio::Device *m_device_1 = domoio::factory_device("foo", "password", true);
  m_device_1->save();
  domoio::Device *m_device_2 = domoio::factory_device("bar", "password");
  m_device_2->save();

  // domoio::Device device;
  // device.load_from_db(m_device_1->get_id());
  // LOG(error) << m_device_1->to_json();

  domoio::DeviceState::load_virtual_devices();

  domoio::DeviceState * device_1 = domoio::DeviceState::find(m_device_1->get_id());
  EXPECT_EQ(device_1->id, m_device_1->get_id());

  domoio::DeviceState * device_2 = domoio::DeviceState::find(m_device_2->get_id());
  EXPECT_EQ(NULL, device_2);

  domoio::DeviceState::unload_devices();


  delete(m_device_1);
  delete(m_device_2);
}
