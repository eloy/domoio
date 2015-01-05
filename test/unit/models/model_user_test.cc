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
  ASSERT_EQ(user.get_id(), loaded->get_id());
  ASSERT_EQ("bar", loaded->get_name());
  ASSERT_EQ("user@foo.bar", loaded->get_email());
  delete(loaded);
}



TEST(UserModel, from_json) {
  domoio::User user;
  user.set_name("foo");
  user.set_email("user@foo.bar");
  domoio::User loaded;
  loaded.from_json(user.to_json());

  ASSERT_EQ("foo", loaded.get_name());
  ASSERT_EQ("user@foo.bar", loaded.get_email());
}
