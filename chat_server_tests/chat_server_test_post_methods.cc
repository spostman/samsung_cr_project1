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

TEST_F(ChatServerTest, Post_CreateAccount_Success) {
  ostringstream_t buf;
  // Test for success to create an account
  buf << "account" << UU("?id=")
      << "wsplab" << UU("&password=") << HashString(UU("bestkaistlab"));
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}

TEST_F(ChatServerTest, Post_CreateAccount_Fail_DuplicateId) {
  ostringstream_t buf;
  // Test for duplicate ID
  buf.str(UU(""));
  buf.clear();
  buf << "account" << UU("?id=") << "kaist"
      << UU("&password=") << HashString(UU("12345678"));
  http_response response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str())).get();
  string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Duplicated ID"));
}

TEST_F(ChatServerTest, Post_CreateAccount_Fail_ProhibitedCharInId) {
  ostringstream_t buf;
  // Test for prohibited char in ID
  buf.str(UU(""));
  buf.clear();
  AccountDatabase account_database;
  buf << "account"
      << UU("?id=") << UU("kaist") + kParsingDelimeterAccount
      << UU("&password=") << HashString(UU("12345678"));
  http_response response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str())).get();
  string_t body = 
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Prohibited character in ID"));
}

TEST_F(ChatServerTest, Post_CreateAccount_Fail_ProhibitedCharInPassword) {
  ostringstream_t buf;
  // Test for prohibited char in password
  buf.str(UU(""));
  buf.clear();
  buf << "account" << UU("?id=") << UU("kaist")
      << UU("&password=")
      << UU("12345678" + kParsingDelimeterAccount);
  http_response response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str())).get();
  string_t body = 
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Prohibited character in password"));
}

TEST_F(ChatServerTest, Post_CreateAccount_Fail_NoHttpParameters) {
  ostringstream_t buf;
  // Test for empty parameters
  buf.str(UU(""));
  buf.clear();
  buf << "account";
  http_response response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str())).get();
  string_t body = 
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Account information absence"));
}

TEST_F(ChatServerTest, Post_CreateAccount_Fail_NoPassword) {
  ostringstream_t buf;
  // Test for incomplete password parameter
  buf.str(UU(""));
  buf.clear();
  buf << "account" << UU("?id=") << "wsplab";
  http_response response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str())).get();
  string_t body = 
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Account information absence"));
}

TEST_F(ChatServerTest, Post_CreateAccount_Fail_NoId) {
  ostringstream_t buf;
  // Test for incomplete id parameter
  buf.str(UU(""));
  buf.clear();
  buf << "account" << UU("?password=") << HashString(UU("12345678"));
  http_response response = 
      http_client_->request(http::methods::POST,
                            uri::encode_uri(buf.str())).get();
  string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Account information absence"));
}

TEST_F(ChatServerTest, Post_Login_Success) {
  ostringstream_t buf;
  // Test for success to login
  string_t nonce = GenerateNonce();
  buf << "login" << UU("?id=") << "kaist" << UU("&nonce=") << nonce
      << UU("&password=") << HashLoginPassword(UU("12345678"), nonce);
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);

  buf.str(UU(""));
  buf.clear();
  nonce = GenerateNonce();
  buf << "login" << UU("?id=") << "wsp" << UU("&nonce=") << nonce
      << UU("&password=") << HashLoginPassword(UU("abcdefgh"), nonce);
  response = http_client_->request(http::methods::POST,
                                   uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);

  buf.str(UU(""));
  buf.clear();
  nonce = GenerateNonce();
  buf << "login" << UU("?id=") << "gsis" << UU("&nonce=") << nonce
      << UU("&password=") << HashLoginPassword(UU("!@#$%^&*"), nonce);
  response = http_client_->request(http::methods::POST,
                                   uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}

TEST_F(ChatServerTest, Post_Login_Fail_IdNotExist) {
  ostringstream_t buf;
  // Test for non-existent ID
  buf.str(UU(""));
  buf.clear();
  const string_t nonce = GenerateNonce();
  buf << "login" << UU("?id=") << "yonsei" << UU("&nonce=") << nonce
      << UU("&password=") << HashLoginPassword(UU("12345678"), nonce);
  http_response response =
      http_client_->request(http::methods::POST,
                            uri::encode_uri(buf.str())).get();
  string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("ID not exist"));
}

TEST_F(ChatServerTest, Post_Login_Fail_PasswordError) {
  ostringstream_t buf;
  // Test for password error
  buf.str(UU(""));
  buf.clear();
  string_t nonce = GenerateNonce();
  buf << "login" << UU("?id=") << "kaist" << UU("&nonce=") << nonce
      << UU("&password=") << HashLoginPassword(UU("1234842122"), nonce);
  http_response response = 
      http_client_->request(http::methods::POST,
                            uri::encode_uri(buf.str())).get();
  string_t body = 
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Password error"));

  buf.str(UU(""));
  buf.clear();
  nonce = GenerateNonce();
  buf << "login" << UU("?id=") << "wsp" << UU("&nonce=") << nonce
      << UU("&password=") << HashLoginPassword(UU("abcewjifoew"), nonce);
  response = http_client_->request(http::methods::POST,
                                   uri::encode_uri(buf.str())).get();
  body = response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Password error"));
}

TEST_F(ChatServerTest, Post_Login_Fail_NoParameters) {
  ostringstream_t buf;
  // Test for empty parameters
  buf.str(UU(""));
  buf.clear();
  buf << "login";
  http_response response = 
      http_client_->request(http::methods::POST,
                            uri::encode_uri(buf.str())).get();
  string_t body = 
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Account information absence"));
}

TEST_F(ChatServerTest, Post_Login_Fail_NoPassword) {
  ostringstream_t buf;
  // Test for incomplete password parameter
  buf.str(UU(""));
  buf.clear();
  string_t nonce = GenerateNonce();
  buf << "login" << UU("?id=") << "wsplab" << UU("&nonce=") << nonce;
  http_response response = 
      http_client_->request(http::methods::POST,
                            uri::encode_uri(buf.str())).get();
  string_t body = 
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Account information absence"));
}

TEST_F(ChatServerTest, Post_Login_Fail_NoId) {
  ostringstream_t buf;
  // Test for incomplete id parameter
  buf.str(UU(""));
  buf.clear();
  const string_t nonce = GenerateNonce();
  buf << "login" << UU("?password=") << HashLoginPassword(UU("bestbest"), nonce)
      << UU("&nonce=") << nonce;
  http_response response = 
      http_client_->request(http::methods::POST, 
                            uri::encode_uri(buf.str())).get();
  string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Account information absence"));
}

TEST_F(ChatServerTest, Post_Login_Fail_NotSameNonce) {
  ostringstream_t buf;
  // Test for not using the same nonce
  buf.str(UU(""));
  buf.clear();
  const string_t nonce = GenerateNonce();
  buf << "login" << UU("?id=") << "kaist" << UU("&nonce=") << nonce
      << UU("&password=") << HashLoginPassword(UU("12345678"), GenerateNonce());
  http_response response =
      http_client_->request(http::methods::POST,
          uri::encode_uri(buf.str())).get();
  string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Password error"));
}

TEST_F(ChatServerTest, Post_InputChatMessage_Success) {
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

  // Test for success to input a chat message
  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_message=") << "good day~!"
      << UU("&chat_room=") << "1" << UU("&session_id=") << session_id_;
  response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);

  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_message=") << "good day="
      << UU("&chat_room=") << "2" << UU("&session_id=") << session_id_;
  response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);

  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_message=") << "good day?"
      << UU("&chat_room=") << "3" << UU("&session_id=") << session_id_;
  response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}

TEST_F(ChatServerTest, Post_InputChatMessage_Fail_NoChatMessage) {
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

  //Test for missing chat message parameter
  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_room=") << "1"
      << UU("&session_id=") << session_id_;
  response = http_client_->request(http::methods::POST,
                                   uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
}

TEST_F(ChatServerTest, Post_InputChatMessage_Fail_NoChatRoom) {
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

  //Test for missing chat room parameter
  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_message=") << "good day~!"
      << UU("&session_id=") << session_id_;
  response = http_client_->request(http::methods::POST,
                                   uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
}

TEST_F(ChatServerTest, Post_InputChatMessage_Fail_NoSessionId) {
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

  //Test for missing session id
  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_message=") << "good day?"
      << UU("&chat_room=") << "3";
  response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str())).get();
  string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::Forbidden);
  EXPECT_EQ(body, UU("Not a valid session ID"));
}

TEST_F(ChatServerTest, Post_InputChatMessage_Fail_InvalidSessionId) {
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

  //Test for invalid session id
  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_message=") << "good day?"
      << UU("&chat_room=") << "3" << UU("&session_id=") << "invalid id";
  response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str())).get();
  string_t body = 
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::Forbidden);
  EXPECT_EQ(body, UU("Not a valid session ID"));
}

TEST_F(ChatServerTest, Post_CreateChatRoom_Success) {
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

  // Test for success to make a chat room
  buf.str(UU(""));
  buf.clear();
  buf << "chatroom" << UU("?chat_room=") << "success"
      << UU("&session_id=") << session_id_;
  response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}

TEST_F(ChatServerTest, Post_CreateChatRoom_Fail_Duplicate_ChatRoom) {
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

  //Test for making duplicated chat room
  buf.str(UU(""));
  buf.clear();
  buf << "chatroom" << UU("?chat_room=") << "1"
      << UU("&session_id=") << session_id_;
  response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
}

TEST_F(ChatServerTest, Post_CreateChatRoom_Fail_NoChatRoomParameter) {
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

  //Test for missing chat room parameter
  buf.str(UU(""));
  buf.clear();
  buf << "chatroom" << UU("?session_id=") << session_id_;
  response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
}

TEST_F(ChatServerTest, Post_CreateChatRoom_Fail_InvalidSessionId) {
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

  //Test for invalid session ID
  buf.str(UU(""));
  buf.clear();
  buf << "chatroom" << UU("?chat_room=") << "1"
      << UU("&session_id=") << "invalid";
  response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str())).get();
  string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::Forbidden);
  EXPECT_EQ(body, UU("Not a valid session ID"));
}

TEST_F(ChatServerTest, Post_CreateChatRoom_Fail_NoSessionId) {
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

  //Test for missing session ID
  buf.str(UU(""));
  buf.clear();
  buf << "chatroom" << UU("?chat_room=") << "1";
  response = http_client_->request(http::methods::POST,
                                   uri::encode_uri(buf.str())).get();
  string_t body = 
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::Forbidden);
  EXPECT_EQ(body, UU("Not a valid session ID"));
}