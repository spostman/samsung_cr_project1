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

TEST_F(ChatServerTest, Get_ChatMessage_Success) {
  ostringstream_t buf;
  // Login process for obtaining session ID
  const string_t nonce = GenerateNonce();
  buf << "login" << UU("?id=") << "kaist" << UU("&nonce=") << nonce
      << UU("&password=") << HashLoginPassword(UU("12345678"), nonce);
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  value object = response.extract_json().get();
  const string_t session_id_ = object[UU("session_id")].as_string();

  // Test for getting chat messages for each chat rooms
  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_room=") << "1"
      << UU("&session_id=") << session_id_;
  response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  object = response.extract_json().get();
  json::array chat_list = object.as_array();
  EXPECT_GT(chat_list.size(), static_cast<size_t>(0));

  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_room=") << "2"
      << UU("&session_id=") << session_id_;
  response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  object = response.extract_json().get();
  chat_list = object.as_array();
  EXPECT_GT(chat_list.size(), static_cast<size_t>(0));

  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_room=") << "3"
      << UU("&session_id=") << session_id_;
  response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  object = response.extract_json().get();
  chat_list = object.as_array();
  EXPECT_GT(chat_list.size(), static_cast<size_t>(0));

  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_room=") << "abc"
      << UU("&session_id=") << session_id_;
  response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  object = response.extract_json().get();
  chat_list = object.as_array();
  EXPECT_GT(chat_list.size(), static_cast<size_t>(0));
}

TEST_F(ChatServerTest, Get_ChatMessage_Fail_Invalid_RoomName) {
  ostringstream_t buf;
  // Login process for obtaining session ID
  const string_t nonce = GenerateNonce();
  buf << "login" << UU("?id=") << "kaist" << UU("&nonce=") << nonce
      << UU("&password=") << HashLoginPassword(UU("12345678"), nonce);
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  value object = response.extract_json().get();
  string_t session_id_ = object[UU("session_id")].as_string();

  // Test for invalid room name
  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_room=") << "invalid_room"
      << UU("&session_id=") << session_id_;
  response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
}

TEST_F(ChatServerTest, Get_ChatMessage_Fail_Missing_RoomName) {
  ostringstream_t buf;
  // Login process for obtaining session ID
  const string_t nonce = GenerateNonce();
  buf << "login" << UU("?id=") << "kaist" << UU("&nonce=") << nonce
      << UU("&password=") << HashLoginPassword(UU("12345678"), nonce);
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  value object = response.extract_json().get();
  string_t session_id_ = object[UU("session_id")].as_string();

  // Test for missing room name
  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?session_id=") << session_id_;
  response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
}

TEST_F(ChatServerTest, Get_ChatMessage_Fail_Invalid_SessionId) {
  ostringstream_t buf;
  // Login process for obtaining session ID
  const string_t nonce = GenerateNonce();
  buf << "login" << UU("?id=") << "kaist" << UU("&nonce=") << nonce
      << UU("&password=") << HashLoginPassword(UU("12345678"), nonce);
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  value object = response.extract_json().get();
  string_t session_id_ = object[UU("session_id")].as_string();

  // Test for invalid session id
  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_room=") << "abc"
      << UU("&session_id=") << "invalid";
  response = http_client_->request(http::methods::GET,
    uri::encode_uri(buf.str())).get();
  string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::Forbidden);
  EXPECT_EQ(body, UU("Not a valid session ID"));
}

TEST_F(ChatServerTest, Get_ChatMessage_Fail_Missing_SessionId) {
  ostringstream_t buf;
  // Login process for obtaining session ID
  const string_t nonce = GenerateNonce();
  buf << "login" << UU("?id=") << "kaist" << UU("&nonce=") << nonce
      << UU("&password=") << HashLoginPassword(UU("12345678"), nonce);
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  value object = response.extract_json().get();
  string_t session_id_ = object[UU("session_id")].as_string();

  // Test for missing session id
  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_room=") << "abc";
  response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  string_t body = 
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::Forbidden);
  EXPECT_EQ(body, UU("Not a valid session ID"));
}

TEST_F(ChatServerTest, Get_ChatRoom_Success) {
  ostringstream_t buf;
  // Login process for obtaining session ID
  const string_t nonce = GenerateNonce();
  buf << "login" << UU("?id=") << "kaist" << UU("&nonce=") << nonce
      << UU("&password=") << HashLoginPassword(UU("12345678"), nonce);
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  value object = response.extract_json().get();
  string_t session_id_ = object[UU("session_id")].as_string();

  // Test for success to get chat rooms
  buf.str(UU(""));
  buf.clear();
  buf << "chatroom" << UU("?session_id=") << session_id_;
  response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  object = response.extract_json().get();
  json::array chat_list = object.as_array();
  EXPECT_GT(chat_list.size(), static_cast<size_t>(0));
}

TEST_F(ChatServerTest, Get_ChatRoom_Fail_InvalidSessionId) {
  ostringstream_t buf;
  // Login process for obtaining session ID
  const string_t nonce = GenerateNonce();
  buf << "login" << UU("?id=") << "kaist" << UU("&nonce=") << nonce
      << UU("&password=") << HashLoginPassword(UU("12345678"), nonce);
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  value object = response.extract_json().get();
  string_t session_id_ = object[UU("session_id")].as_string();

  // Test for invalid session id
  buf.str(UU(""));
  buf.clear();
  buf << "chatroom" << UU("?session_id=") << "invalid";
  response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::Forbidden);
  EXPECT_EQ(body, UU("Not a valid session ID"));
}

TEST_F(ChatServerTest, Get_ChatRoom_Fail_Missing_SessionId) {
  ostringstream_t buf;
  // Login process for obtaining session ID
  const string_t nonce = GenerateNonce();
  buf << "login" << UU("?id=") << "kaist" << UU("&nonce=") << nonce
      << UU("&password=") << HashLoginPassword(UU("12345678"), nonce);
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  value object = response.extract_json().get();
  string_t session_id_ = object[UU("session_id")].as_string();

  // Test for missing session id
  buf.str(UU(""));
  buf.clear();
  buf << "chatroom";
  response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  string_t body = 
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::Forbidden);
  EXPECT_EQ(body, UU("Not a valid session ID"));
}