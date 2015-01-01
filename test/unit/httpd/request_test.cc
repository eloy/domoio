#include "domoio_test.h"
#include "httpd.h"

bool request_test_callback(domoio::httpd::Request *request) {
  return true;
}

TEST(Request, extract_params) {
  domoio::httpd::register_action("/foo/?:foo_id?", &request_test_callback);
  domoio::httpd::Request request("GET", "/foo/1");
  request.resolve();
  ASSERT_STREQ("1", request.param("foo_id"));
  ASSERT_EQ(0, request.param_int("bar"));
  ASSERT_EQ(1, request.param_int("foo_id"));
}
