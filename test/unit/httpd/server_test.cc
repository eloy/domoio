#include "domoio_test.h"
#include "httpd.h"

// namespace domoio {
//   namespace httpd {
//     HttpdAction *find_action(const char*);
//     bool register_action(const char*, HttpdCallback);
//     void run_action(HttpdAction *, const char*);
//     namespace actions {
//       int action_foo() {
//         return 1;
//       }
//       int action_bar() {
//         return 2;
//       }
//       int action_wadus() {
//         return 3;
//       }
//     }

//   }
// }



// TEST(HttpdServer, action_not_found) {
//   domoio::httpd::HttpdAction* action = domoio::httpd::find_action("/test");
//   EXPECT_EQ(NULL, action);
// }

// TEST(HttpdServer, find_action) {

//   domoio::httpd::register_action("/api/foo/?(\\d*)", &domoio::httpd::actions::action_foo);
//   domoio::httpd::register_action("/api/bar/?(\\d*)", &domoio::httpd::actions::action_bar);
//   domoio::httpd::register_action("/api/bar/?(\\d+)/wadus/?(\\d*)", &domoio::httpd::actions::action_wadus);


//   domoio::httpd::HttpdAction* action = domoio::httpd::find_action("/test");
//   EXPECT_EQ(NULL, action);

//   action = domoio::httpd::find_action("/api/foo");
//   EXPECT_EQ(action->callback(), 1);

//   action = domoio::httpd::find_action("/api/foo/1");
//   EXPECT_EQ(action->callback(), 1);

//   action = domoio::httpd::find_action("/api/bar");
//   EXPECT_EQ(action->callback(), 2);

//   action = domoio::httpd::find_action("/api/bar/5");
//   EXPECT_EQ(action->callback(), 2);

//   action = domoio::httpd::find_action("/api/bar/5/wadus");
//   EXPECT_EQ(action->callback(), 3);

//   action = domoio::httpd::find_action("/api/bar/5/wadus/1");
//   EXPECT_EQ(action->callback(), 3);
// }


// TEST(HttpdServer, run_action) {

//   domoio::httpd::register_action("/api/bar/?(\\d+)/wadus/?(\\d*)", &domoio::httpd::actions::action_wadus);

//   const char* url = "/api/bar/1/wadus/2";
//   domoio::httpd::HttpdAction* action = domoio::httpd::find_action(url);
//   domoio::httpd::run_action(action, url);
// }
