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
using ::web::uri;

TEST_F(ChatServerTest, Post_CreateAccount_Success) {
  ostringstream_t buf;
  buf << UU("account");
  value body_data;
  body_data[UU("id")] = value::string(UU("wsplab"));
  body_data[UU("password")] = value::string(HashString(UU("bestkaistlab")));
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}

TEST_F(ChatServerTest, Post_CreateAccount_Success_whitespace) {
  ostringstream_t buf;
  buf << UU("account");
  value body_data;
  body_data[UU("id")] = value::string(UU("ws p    l ab"));
  body_data[UU("password")] = value::string(HashString(UU("bestkaistlab")));
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}

TEST_F(ChatServerTest, Post_CreateAccount_Success_Special_Characters) {
  ostringstream_t buf;
  buf << UU("account");
  value body_data;
  body_data[UU("id")] = value::string(UU("ws!@$!@%=-#$#$^"));
  body_data[UU("password")] = value::string(HashString(UU("bestkaistlab")));
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);

  body_data[UU("id")] = value::string(UU("./;'[]123"));
  body_data[UU("password")] = value::string(HashString(UU("bestkaistlab")));
  response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}

TEST_F(ChatServerTest, Post_CreateAccount_Success_Numbers) {
  ostringstream_t buf;
  buf << UU("account");
  value body_data;
  body_data[UU("id")] = value::string(UU("34509785468120"));
  body_data[UU("password")] = value::string(HashString(UU("bestkaistlab")));
  http_response response = http_client_->request(
    http::methods::POST, uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}

TEST_F(ChatServerTest, Post_CreateAccount_Fail_DuplicateId) {
  // Test for duplicate ID.
  ostringstream_t buf;
  buf << UU("account");
  value body_data;
  body_data[UU("id")] = value::string(UU("kaist"));
  body_data[UU("password")] = value::string(HashString(UU("12345678")));
  http_response response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str()), body_data).get();
  string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Duplicated ID"));
}

TEST_F(ChatServerTest, Post_CreateAccount_Fail_ProhibitedCharInId) {
  // Test for prohibited char in ID.
  ostringstream_t buf;
  buf << UU("account");
  value body_data;
  body_data[UU("id")] = value::string(UU("yonsei") + kParsingDelimeterAccount);
  body_data[UU("password")] = value::string(HashString(UU("12345678")));
  http_response response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str()), body_data).get();
  string_t body = 
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Prohibited character in ID"));

  body_data[UU("id")] = value::string(UU("yonsei") + kParsingDelimeterChatDb);
  body_data[UU("password")] = value::string(HashString(UU("12345678")));
  response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str()), body_data).get();
  body = response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Prohibited character in ID"));
}

TEST_F(ChatServerTest, Post_CreateAccount_Fail_ProhibitedCharInPassword) {
  // Test for prohibited char in password.
  ostringstream_t buf;
  buf << UU("account");
  value body_data;
  body_data[UU("id")] = value::string(UU("yonsei"));
  body_data[UU("password")] = value::string(HashString(UU("12345678")) + 
                                            kParsingDelimeterAccount);
  http_response response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str()), body_data).get();
  string_t body = 
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Prohibited character in password"));
}

TEST_F(ChatServerTest, Post_CreateAccount_Fail_NoHttpParameters) {
  // Test for empty parameters.
  ostringstream_t buf;
  buf << UU("account");
  http_response response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str())).get();
  string_t body = 
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Account information absence"));
}

TEST_F(ChatServerTest, Post_CreateAccount_Fail_NoPassword) {
  // Test for no password.
  ostringstream_t buf;
  buf << UU("account");
  value body_data;
  body_data[UU("id")] = value::string(UU("kaist"));
  http_response response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str()), body_data).get();
  string_t body = 
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Account information absence"));
}

TEST_F(ChatServerTest, Post_CreateAccount_Fail_NoId) {
  // Test for no id.
  ostringstream_t buf;
  buf << UU("account");
  value body_data;
  body_data[UU("password")] = value::string(UU("12345678"));
  http_response response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str()), body_data).get();
  string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Account information absence"));
}

TEST_F(ChatServerTest, Post_Login_Success) {
  // Test for success to login.
  ostringstream_t buf;
  buf << UU("login");
  value body_data;
  string_t nonce = GenerateNonce();
  body_data[UU("id")] = value::string(UU("kaist"));
  body_data[UU("nonce")] = value::string(nonce);
  body_data[UU("password")] = 
      value::string(HashLoginPassword(UU("12345678"), nonce));
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);

  nonce = GenerateNonce();
  body_data[UU("id")] = value::string(UU("wsp"));
  body_data[UU("nonce")] = value::string(nonce);
  body_data[UU("password")] =
      value::string(HashLoginPassword(UU("abcdefgh"), nonce));
  response = http_client_->request(http::methods::POST,
                                   uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);

  nonce = GenerateNonce();
  body_data[UU("id")] = value::string(UU("gsis"));
  body_data[UU("nonce")] = value::string(nonce);
  body_data[UU("password")] =
      value::string(HashLoginPassword(UU("!@#$%^&*"), nonce));
  response = http_client_->request(http::methods::POST,
                                   uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}

TEST_F(ChatServerTest, Post_Login_Success_Whitespace) {
  // Test for success to login.
  ostringstream_t buf;
  buf << UU("login");
  value body_data;
  string_t nonce = GenerateNonce();
  body_data[UU("id")] = value::string(UU("lab  w sp"));
  body_data[UU("nonce")] = value::string(nonce);
  body_data[UU("password")] =
      value::string(HashLoginPassword(UU("12345678"), nonce));
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}

TEST_F(ChatServerTest, Post_Login_Success_SpecialChar) {
  // Test for success to login.
  ostringstream_t buf;
  buf << UU("login");
  value body_data;
  string_t nonce = GenerateNonce();
  body_data[UU("id")] = value::string(UU("lab !@.#$% hi"));
  body_data[UU("nonce")] = value::string(nonce);
  body_data[UU("password")] =
      value::string(HashLoginPassword(UU("!@#$%^&*"), nonce));
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}

TEST_F(ChatServerTest, Post_Login_Fail_IdNotExist) {
  // Test for no id.
  ostringstream_t buf;
  buf << UU("login");
  value body_data;
  string_t nonce = GenerateNonce();
  body_data[UU("id")] = value::string(UU("yonsei"));
  body_data[UU("nonce")] = value::string(nonce);
  body_data[UU("password")] =
      value::string(HashLoginPassword(UU("12345678"), nonce));
  http_response response =
      http_client_->request(http::methods::POST,
                            uri::encode_uri(buf.str()), body_data).get();
  string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("ID not exist"));
}

TEST_F(ChatServerTest, Post_Login_Fail_PasswordError) {
  // Test for password error.
  ostringstream_t buf;
  buf << UU("login");
  value body_data;
  string_t nonce = GenerateNonce();
  body_data[UU("id")] = value::string(UU("kaist"));
  body_data[UU("nonce")] = value::string(nonce);
  body_data[UU("password")] =
      value::string(HashLoginPassword(UU("12345678489ffwe"), nonce));
  http_response response = 
      http_client_->request(http::methods::POST,
                            uri::encode_uri(buf.str()), body_data).get();
  string_t body = 
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Password error"));

  nonce = GenerateNonce();
  body_data[UU("id")] = value::string(UU("wsp"));
  body_data[UU("nonce")] = value::string(nonce);
  body_data[UU("password")] =
      value::string(HashLoginPassword(UU("abewfwefsde"), nonce));
  response = http_client_->request(http::methods::POST,
                                   uri::encode_uri(buf.str()), body_data).get();
  body = response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Password error"));
}

TEST_F(ChatServerTest, Post_Login_Fail_NoParameters) {
  // Test for empty parameters.
  ostringstream_t buf;
  buf << UU("login");
  http_response response = 
      http_client_->request(http::methods::POST,
                            uri::encode_uri(buf.str())).get();
  string_t body = 
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Account information absence"));
}

TEST_F(ChatServerTest, Post_Login_Fail_NoId) {
  // Test for no id.
  ostringstream_t buf;
  buf << UU("login");
  value body_data;
  string_t nonce = GenerateNonce();
  body_data[UU("password")] = 
      value::string(HashLoginPassword(UU("12345678"), nonce));
  body_data[UU("nonce")] = value::string(nonce);
  http_response response =
      http_client_->request(http::methods::POST,
                            uri::encode_uri(buf.str()), body_data).get();
  string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Account information absence"));
}

TEST_F(ChatServerTest, Post_Login_Fail_NoPassword) {
  // Test for no password.
  ostringstream_t buf;
  buf << UU("login");
  value body_data;
  string_t nonce = GenerateNonce();
  body_data[UU("id")] = value::string(UU("kaist"));
  body_data[UU("nonce")] = value::string(nonce);
  http_response response = 
      http_client_->request(http::methods::POST,
                            uri::encode_uri(buf.str()), body_data).get();
  string_t body = 
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Account information absence"));
}

TEST_F(ChatServerTest, Post_Login_Fail_NoNonce) {
  // Test for no nonce.
  ostringstream_t buf;
  buf << UU("login");
  value body_data;
  string_t nonce = GenerateNonce();
  body_data[UU("id")] = value::string(UU("kaist"));
  body_data[UU("password")] =
      value::string(HashLoginPassword(UU("12345678"), nonce));
  http_response response =
      http_client_->request(http::methods::POST,
                            uri::encode_uri(buf.str()), body_data).get();
  string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Account information absence"));
}

TEST_F(ChatServerTest, Post_Login_Fail_NotSameNonce) {
  // Test for not using the same nonce.
  ostringstream_t buf;
  buf << UU("login");
  value body_data;
  string_t nonce = GenerateNonce();
  body_data[UU("id")] = value::string(UU("kaist"));
  body_data[UU("password")] =
    value::string(HashLoginPassword(UU("12345678"), nonce));
  body_data[UU("nonce")] = value::string(GenerateNonce());
  http_response response =
      http_client_->request(http::methods::POST,
          uri::encode_uri(buf.str()), body_data).get();
  string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Password error"));
}

TEST_F(ChatServerTest, Post_InputChatMessage_Success) {
  string_t session_id = PerformSuccessfulLogin();
  // Test for success to input a chat message.
  ostringstream_t buf;
  buf << UU("chatmessage");
  value body_data;
  body_data[UU("chat_message")] = value::string(UU("good day~!"));
  body_data[UU("chat_room")] = value::string(UU("1"));
  body_data[UU("session_id")] = value::string(session_id);
  http_response response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);

  body_data[UU("chat_message")] = value::string(UU("good day="));
  body_data[UU("chat_room")] = value::string(UU("2"));
  body_data[UU("session_id")] = value::string(session_id);
  response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);

  body_data[UU("chat_message")] = value::string(UU("good da?"));
  body_data[UU("chat_room")] = value::string(UU("3"));
  body_data[UU("session_id")] = value::string(session_id);
  response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}

TEST_F(ChatServerTest, Post_InputChatMessage_Success_Whitespace) {
  string_t session_id = PerformSuccessfulLogin();
  // Test for success to input a chat message with whitespace.
  ostringstream_t buf;
  buf << UU("chatmessage");
  value body_data;
  body_data[UU("chat_message")] = value::string(UU("good day~!  hihi    "));
  body_data[UU("chat_room")] = value::string(UU("1"));
  body_data[UU("session_id")] = value::string(session_id);
  http_response response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}

TEST_F(ChatServerTest, Post_InputChatMessage_Success_SpecialChar) {
  string_t session_id = PerformSuccessfulLogin();
  // Test for success to input a chat message with special char.
  ostringstream_t buf;
  buf << UU("chatmessage");
  value body_data;
  body_data[UU("chat_message")] = value::string(UU("!@#%^&*)!@#./;'[]{}\!"));
  body_data[UU("chat_room")] = value::string(UU("1"));
  body_data[UU("session_id")] = value::string(session_id);
  http_response response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}

TEST_F(ChatServerTest, Post_InputChatMessage_Fail_NoChatMessage) {
  string_t session_id = PerformSuccessfulLogin();
  //Test for missing chat message.
  ostringstream_t buf;
  buf << UU("chatmessage");
  value body_data;
  body_data[UU("chat_room")] = value::string(UU("1"));
  body_data[UU("session_id")] = value::string(session_id);
  http_response response = 
      http_client_->request(http::methods::POST,
                            uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
}

TEST_F(ChatServerTest, Post_InputChatMessage_Fail_NoChatRoom) {
  string_t session_id = PerformSuccessfulLogin();
  //Test for missing chat room.
  ostringstream_t buf;
  buf << UU("chatmessage");
  value body_data;
  body_data[UU("chat_message")] = value::string(UU("good day~!"));
  body_data[UU("session_id")] = value::string(session_id);
  http_response response = 
      http_client_->request(http::methods::POST,
                            uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
}

TEST_F(ChatServerTest, Post_InputChatMessage_Fail_NoSessionId) {
  string_t session_id = PerformSuccessfulLogin();
  // Test for missing session id.
  ostringstream_t buf;
  buf << UU("chatmessage");
  value body_data;
  body_data[UU("chat_message")] = value::string(UU("good day~!"));
  body_data[UU("chat_room")] = value::string(UU("1"));
  http_response response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str()), body_data).get();
  string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::Forbidden);
  EXPECT_EQ(body, UU("Not a valid session ID"));
}

TEST_F(ChatServerTest, Post_InputChatMessage_Fail_InvalidSessionId) {
  string_t session_id = PerformSuccessfulLogin();
  // Test for invalid session id.
  ostringstream_t buf;
  buf << UU("chatmessage");
  value body_data;
  body_data[UU("chat_message")] = value::string(UU("good day~!"));
  body_data[UU("chat_room")] = value::string(UU("1"));
  body_data[UU("session_id")] = value::string(UU("invalid session id"));
  http_response response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str()), body_data).get();
  string_t body = 
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::Forbidden);
  EXPECT_EQ(body, UU("Not a valid session ID"));
}

TEST_F(ChatServerTest, Post_InputChatMessage_Fail_Prohibited_Char) {
  string_t session_id = PerformSuccessfulLogin();
  // Test for chat message with prohibited char.
  ostringstream_t buf;
  buf << UU("chatmessage");
  value body_data;
  body_data[UU("chat_message")] = value::string(UU("||good day~!||"));
  body_data[UU("chat_room")] = value::string(UU("1"));
  body_data[UU("session_id")] = value::string(session_id);
  http_response response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str()), body_data).get();
  string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
  EXPECT_EQ(body, UU("Prohibited char in the message, room, or date"));
}

TEST_F(ChatServerTest, Post_CreateChatRoom_Success) {
  string_t session_id = PerformSuccessfulLogin();
  // Test for success to make a chat room
  ostringstream_t buf;
  buf << UU("chatroom");
  value body_data;
  body_data[UU("chat_room")] = value::string(UU("12345"));
  body_data[UU("session_id")] = value::string(session_id);
  http_response response = 
      http_client_->request(http::methods::POST,
                            uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}

TEST_F(ChatServerTest, Post_CreateChatRoom_Success_Whitespace) {
  string_t session_id = PerformSuccessfulLogin();
  // Test for success to make a chat room with whitespace
  ostringstream_t buf;
  buf << UU("chatroom");
  value body_data;
  body_data[UU("chat_room")] = value::string(UU("12 3   45"));
  body_data[UU("session_id")] = value::string(session_id);
  http_response response =
      http_client_->request(http::methods::POST,
          uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}

TEST_F(ChatServerTest, Post_CreateChatRoom_Success_SpecialChar) {
  string_t session_id = PerformSuccessfulLogin();
  // Test for success to make a chat room
  ostringstream_t buf;
  buf << UU("chatroom");
  value body_data;
  body_data[UU("chat_room")] = value::string(UU("!@#$%^&*()_+"));
  body_data[UU("session_id")] = value::string(session_id);
  http_response response =
      http_client_->request(http::methods::POST,
          uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}

TEST_F(ChatServerTest, Post_CreateChatRoom_Fail_ProhibitedChar) {
  string_t session_id = PerformSuccessfulLogin();
  // Test for success to make a chat room
  ostringstream_t buf;
  buf << UU("chatroom");
  value body_data;
  body_data[UU("chat_room")] = value::string(UU("abcdef") + 
                                             kParsingDelimeterChatDb);
  body_data[UU("session_id")] = value::string(session_id);
  http_response response =
    http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
}

TEST_F(ChatServerTest, Post_CreateChatRoom_Fail_Duplicate_ChatRoom) {
  string_t session_id = PerformSuccessfulLogin();
  //Test for making duplicated chat room
  ostringstream_t buf;
  buf << UU("chatroom");
  value body_data;
  body_data[UU("chat_room")] = value::string(UU("1"));
  body_data[UU("session_id")] = value::string(session_id);
  http_response response =
      http_client_->request(http::methods::POST,
          uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
}

TEST_F(ChatServerTest, Post_CreateChatRoom_Fail_NoChatRoomParameter) {
  string_t session_id = PerformSuccessfulLogin();
  //Test for missing chat room parameter
  ostringstream_t buf;
  buf << UU("chatroom");
  value body_data;
  body_data[UU("session_id")] = value::string(session_id);
  http_response response =
      http_client_->request(http::methods::POST,
          uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
}

TEST_F(ChatServerTest, Post_CreateChatRoom_Fail_InvalidSessionId) {
  string_t session_id = PerformSuccessfulLogin();
  //Test for invalid session ID
  ostringstream_t buf;
  buf << UU("chatroom");
  value body_data;
  body_data[UU("chat_room")] = value::string(UU("987"));
  body_data[UU("session_id")] = value::string(UU("invalid session id"));
  http_response response =
      http_client_->request(http::methods::POST,
          uri::encode_uri(buf.str()), body_data).get();
  string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::Forbidden);
  EXPECT_EQ(body, UU("Not a valid session ID"));
}

TEST_F(ChatServerTest, Post_CreateChatRoom_Fail_NoSessionId) {
  string_t session_id = PerformSuccessfulLogin();
  //Test for missing session ID
  ostringstream_t buf;
  buf << UU("chatroom");
  value body_data;
  body_data[UU("chat_room")] = value::string(UU("987"));
  http_response response =
      http_client_->request(http::methods::POST,
          uri::encode_uri(buf.str()), body_data).get();
  string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::Forbidden);
  EXPECT_EQ(body, UU("Not a valid session ID"));
}