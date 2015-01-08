#include "domoio_test.h"


class DatabaseCleaner : public ::testing::EmptyTestEventListener {
  // Called before a test starts.
  virtual void OnTestStart(const ::testing::TestInfo& test_info) {
    domoio::db::exec_update("truncate table users");
    domoio::db::exec_update("truncate table devices");
  }

  // Called after a failed assertion or a SUCCEED() invocation.
  virtual void OnTestPartResult(const ::testing::TestPartResult& test_part_result) {
  }

  // Called after a test ends.
  virtual void OnTestEnd(const ::testing::TestInfo& test_info) {
  }
};



class TestEnvironment : public ::testing::Environment {
public:
  virtual ~TestEnvironment() {}

  virtual void SetUp() {
    domoio::init_domoio();
  }


  virtual void TearDown() {
    domoio::exit_domoio();
  }
};


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::Environment* const env = ::testing::AddGlobalTestEnvironment(new TestEnvironment);

  domoio::setup_config_options("config_test");
  domoio::prepare_config(argc, argv);

  // Gets hold of the event listener list.
  ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();
  // Adds a listener to the end.  Google Test takes the ownership.
  listeners.Append(new DatabaseCleaner);

  return RUN_ALL_TESTS();
}
