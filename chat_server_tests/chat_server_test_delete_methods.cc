// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#include "chat_server.h"
#include "gtest/gtest.h"
#include "cpprest/http_client.h"
#include "chat_server_test_fixture.h"

using namespace std;
using namespace utility;
using namespace chatserver;
using namespace chatservertests;
using namespace web;
using ::web::http::http_response;

TEST_F(ChatServerTest, Delete_Logout_Success) {
  const string_t session_id = PerformSuccessfulLogin();
  // Test success to logout.
  ostringstream_t buf;
  buf.str(UU(""));
  buf.clear();
  buf << "session" << UU("?session_id=") << session_id;
  http_response response = http_client_->request(http::methods::DEL,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}

TEST_F(ChatServerTest, Delete_Logout_Fail_InvalidSession) {
  const string_t session_id = PerformSuccessfulLogin();
  // Test for invalid session id.
  ostringstream_t buf;
  buf.str(UU(""));
  buf.clear();
  buf << "session" << UU("?session_id=") << "invalid session";
  http_response response = http_client_->request(http::methods::DEL,
      uri::encode_uri(buf.str())).get();
  const string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::Forbidden);
  EXPECT_EQ(body, UU("Not a valid session ID"));
}

TEST_F(ChatServerTest, Delete_Logout_Fail_MissingSession) {
  const string_t session_id = PerformSuccessfulLogin();
  // Test for missing session id.
  ostringstream_t buf;
  buf.str(UU(""));
  buf.clear();
  buf << "session";
  http_response response = http_client_->request(http::methods::DEL,
      uri::encode_uri(buf.str())).get();
  const string_t body = 
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::Forbidden);
  EXPECT_EQ(body, UU("Not a valid session ID"));
}