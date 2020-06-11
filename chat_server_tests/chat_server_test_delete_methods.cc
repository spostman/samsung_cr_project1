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
using ::concurrency::task_status;
using ::web::http::client::http_client;
using ::web::http::http_response;
using ::web::json::value;

TEST_F(ChatServerTest, Delete_Logout_Success) {
  // Login process for obtaining session ID
  ostringstream_t buf;
  const string_t nonce = GenerateNonce();
  buf << "login" << UU("?id=") << "kaist" << UU("&nonce=") << nonce
      << UU("&password=") << HashLoginPassword(UU("12345678"), nonce);
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  value object = response.extract_json().get();
  const string_t session_id_ = object[UU("session_id")].as_string();

  // Test success to logout
  buf.str(UU(""));
  buf.clear();
  buf << "session" << UU("?session_id=") << session_id_;
  response = http_client_->request(http::methods::DEL,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}

TEST_F(ChatServerTest, Delete_Logout_Fail_InvalidSession) {
  // Login process for obtaining session ID
  ostringstream_t buf;
  const string_t nonce = GenerateNonce();
  buf << "login" << UU("?id=") << "kaist" << UU("&nonce=") << nonce
      << UU("&password=") << HashLoginPassword(UU("12345678"), nonce);
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  value object = response.extract_json().get();
  const string_t session_id_ = object[UU("session_id")].as_string();

  // Test for invalid session id
  buf.str(UU(""));
  buf.clear();
  buf << "session" << UU("?session_id=") << "invalid session";
  response = http_client_->request(http::methods::DEL,
      uri::encode_uri(buf.str())).get();
  const string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::Forbidden);
  EXPECT_EQ(body, UU("Not a valid session ID"));
}

TEST_F(ChatServerTest, Delete_Logout_Fail_MissingSession) {
  // Login process for obtaining session ID
  ostringstream_t buf;
  const string_t nonce = GenerateNonce();
  buf << "login" << UU("?id=") << "kaist" << UU("&nonce=") << nonce
      << UU("&password=") << HashLoginPassword(UU("12345678"), nonce);
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  value object = response.extract_json().get();
  const string_t session_id_ = object[UU("session_id")].as_string();

  // Test for missing session id
  buf.str(UU(""));
  buf.clear();
  buf << "session";
  response = http_client_->request(http::methods::DEL,
      uri::encode_uri(buf.str())).get();
  const string_t body = 
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::Forbidden);
  EXPECT_EQ(body, UU("Not a valid session ID"));
}