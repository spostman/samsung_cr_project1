// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#include "gtest/gtest.h"
#include <chrono>
#include <thread>
#include "session_manager.h"

#include "session.h"

using namespace std;
using namespace chrono;
using namespace utility;
using namespace chatserver;

// Fixture class for session_manager.h testing.
class SessionManagerTest : public ::testing::Test {
 protected:
  SessionManager session_manager_;
  Session kaist_session;
  Session wsp_session;
  Session gsis_session;

  void SetUp() override {
    kaist_session = session_manager_.CreateSession(UU("kaist"));
    wsp_session = session_manager_.CreateSession(UU("wsp"));
    gsis_session = session_manager_.CreateSession(UU("gsis"));
  }
};

TEST_F(SessionManagerTest, IsExistSessionId_Success) {
  // Check given id is exist in the server session.
  EXPECT_EQ(true, session_manager_.IsExistSessionId(kaist_session.session_id));
  EXPECT_EQ(true, session_manager_.IsExistSessionId(wsp_session.session_id));
  EXPECT_EQ(true, session_manager_.IsExistSessionId(gsis_session.session_id));
}

TEST_F(SessionManagerTest, IsExistSessionId_Fail) {
  // Check given id is not exist in the server session.
  EXPECT_EQ(false, session_manager_.IsExistSessionId(UU("ABCDEFG")));
}

TEST_F(SessionManagerTest, CreateSession_Success) {
  const Session session = session_manager_.CreateSession(UU("n5"));
  const string_t empty = UU("");

  // Check created session.
  EXPECT_STRNE(session.session_id.c_str(), empty.c_str());
  EXPECT_STRNE(session.user_id.c_str(), empty.c_str());
  EXPECT_NE(session.last_activity_time, 0);

  // Re-create existing sessions.
  string_t session_id = kaist_session.session_id;
  EXPECT_STREQ(session_id.c_str(),
               session_manager_.CreateSession(UU("kaist")).session_id.c_str());

  session_id = wsp_session.session_id;
  EXPECT_STREQ(session_id.c_str(),
               session_manager_.CreateSession(UU("wsp")).session_id.c_str());

  session_id = gsis_session.session_id;
  EXPECT_STREQ(session_id.c_str(),
               session_manager_.CreateSession(UU("gsis")).session_id.c_str());
}

TEST_F(SessionManagerTest, DeleteSession_Success_And_Fail) {
  // Delete exist sessions twice.
  EXPECT_EQ(true, session_manager_.DeleteSession(kaist_session.session_id));
  EXPECT_EQ(false, session_manager_.DeleteSession(kaist_session.session_id));
  EXPECT_EQ(true, session_manager_.DeleteSession(wsp_session.session_id));
  EXPECT_EQ(false, session_manager_.DeleteSession(wsp_session.session_id));
  EXPECT_EQ(true, session_manager_.DeleteSession(gsis_session.session_id));
  EXPECT_EQ(false, session_manager_.DeleteSession(gsis_session.session_id));
}

TEST_F(SessionManagerTest, DeleteSession_Fail_NoSessionExist) {
  // Delete the session that does not exist.
  EXPECT_EQ(
      false, session_manager_.DeleteSession(UU("ABC123890jfiuw1``78fsd9j823r98")));
}

TEST_F(SessionManagerTest, RenewLastAcitvityTime_Success) {
  // Renew exist sessions.
  EXPECT_EQ(
      true, session_manager_.RenewLastActivityTime(kaist_session.session_id));
  EXPECT_EQ(
      true, session_manager_.RenewLastActivityTime(wsp_session.session_id));
  EXPECT_EQ(
      true, session_manager_.RenewLastActivityTime(gsis_session.session_id));
}

TEST_F(SessionManagerTest, RenewLastAcitvityTime_Fail) {
  // Renew nonexistent sessions.
  EXPECT_EQ(
      false, session_manager_.DeleteSession(UU("ABC123890jfiuw1278fsd9j823r98")));
}

TEST_F(SessionManagerTest, GetAccountIdFromSessionId_Success) {
  string_t session_id;
  // Get a user ID from a session ID.
  // Check the ID is correct.
  EXPECT_EQ(
      true, session_manager_.GetUserIDFromSessionId(kaist_session.session_id,
      &session_id));
  EXPECT_STREQ(kaist_session.user_id.c_str(), session_id.c_str());
  EXPECT_EQ(
      true, session_manager_.GetUserIDFromSessionId(wsp_session.session_id,
      &session_id));
  EXPECT_STREQ(wsp_session.user_id.c_str(), session_id.c_str());
  EXPECT_EQ(
      true, session_manager_.GetUserIDFromSessionId(gsis_session.session_id,
      &session_id));
  EXPECT_STREQ(gsis_session.user_id.c_str(), session_id.c_str());
}

TEST_F(SessionManagerTest, GetAccountIdFromSessionId_Fail) {
  // Check nullptr error.
  EXPECT_EQ(
      false, session_manager_.GetUserIDFromSessionId(gsis_session.session_id,
      nullptr));
}

TEST_F(SessionManagerTest, RunSessionExpireThread) {
  // Check session existence.
  session_manager_.RunSessionExpireThread();
  EXPECT_EQ(true, session_manager_.IsExistSessionId(kaist_session.session_id));
  EXPECT_EQ(true, session_manager_.IsExistSessionId(wsp_session.session_id));
  EXPECT_EQ(true, session_manager_.IsExistSessionId(gsis_session.session_id));

  // Wait and check if session exists.
  const time_t wait_time = 2;
  const seconds interval(wait_time);
  this_thread::sleep_for(duration_cast<seconds>(interval));
  EXPECT_EQ(true, session_manager_.IsExistSessionId(kaist_session.session_id));
  EXPECT_EQ(true, session_manager_.IsExistSessionId(wsp_session.session_id));
  EXPECT_EQ(true, session_manager_.IsExistSessionId(gsis_session.session_id));
}

// Check how unique the Session ID is created
// Test that the same session ID is not created,
// even if a large number of session IDs are generated
// Create 10,000 session IDs and check if they have the same ID
TEST_F(SessionManagerTest, GenerateSessionID) {
  set<string_t> session_id_set;
  for (auto i = 0; i < 1000; i++) {
    Session session = session_manager_.CreateSession(to_wstring(i));
    EXPECT_EQ(true,
              session_id_set.find(session.session_id) == session_id_set.end());
    session_id_set.insert(session.session_id);
  }
}