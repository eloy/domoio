#include "domoio_test.h"
#include "httpd.h"

bool action_test_callback(domoio::httpd::Request *request) {
  return true;
}

TEST(httpd_action, extract_params_index) {
  domoio::httpd::HttpdAction action("/foo/:foo_id/:id", &action_test_callback);
  ASSERT_EQ("foo_id", action.params_index[1]);
  ASSERT_EQ("id", action.params_index[2]);
}

TEST(httpd_action, build_regexp) {
  domoio::httpd::HttpdAction action("/foo/:foo_id/?:id?", &action_test_callback);
  ASSERT_TRUE(regex_match("/foo/1/2", action.regexp));
  ASSERT_TRUE(regex_match("/foo/1", action.regexp));
  ASSERT_FALSE(regex_match("/foo", action.regexp));
  ASSERT_FALSE(regex_match("/bar/1", action.regexp));
  ASSERT_FALSE(regex_match("/bar/1/2", action.regexp));
  ASSERT_FALSE(regex_match("", action.regexp));
}
