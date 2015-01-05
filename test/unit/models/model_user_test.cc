#include "domoio_test.h"
#include "data_mapper.h"

TEST(UserModel, save) {
  domoio::User user;
  user.set_name("foo");
  user.set_email("user@foo.bar");
  user.save();
  user.set_name("bar");
  user.save();

  domoio::User *loaded = domoio::User::find(user.get_id());
  ASSERT_EQ(loaded->get_id(), user.get_id());
  ASSERT_EQ(loaded->get_name(), "bar");
  ASSERT_EQ(loaded->get_email(), "user@foo.bar");
  delete(loaded);
}
