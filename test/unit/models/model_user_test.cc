#include <gtest/gtest.h>
#include "models.h"


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


TEST(UserModel, mem) {
  domoio::User foo, bar;
  foo.set_name("foo");
  foo.set_email("foo@domoio.com");
  foo.save();

  bar.set_name("bar");
  bar.set_email("bar@domoio.com");
  bar.save();

  vault::ModelsCollection<domoio::User> collection;
  domoio::User::all(&collection);

  domoio::User *first = collection.at(0);
  domoio::User *last = collection.at(1);
  ASSERT_EQ("foo", first->get_name());
  ASSERT_EQ("bar", last->get_name());
}

TEST(UserModel, clone) {
  // domoio::User *foo = new domoio::User();
  // foo->set_name("foo");
  // foo->set_email("foo@domoio.com");

  // domoio::User *bar;
  // bar = foo->clone();
  // delete(foo);
  // ASSERT_EQ("foo", bar->get_name());
  // ASSERT_EQ("foo@domoio.com", bar->get_email());
  // delete(bar);
}
