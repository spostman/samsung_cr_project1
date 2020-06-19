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
  const string_t session_id = PerformSuccessfulLogin();
  // Test for getting chat messages for each chat rooms
  ostringstream_t buf;
  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_room=") << "1"
      << UU("&session_id=") << session_id;
  http_response response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  json::array chat_list = response.extract_json().get().as_array();
  EXPECT_GT(chat_list.size(), static_cast<size_t>(0));

  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_room=") << "2"
      << UU("&session_id=") << session_id;
  response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  chat_list = response.extract_json().get().as_array();
  EXPECT_GT(chat_list.size(), static_cast<size_t>(0));

  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_room=") << "3"
      << UU("&session_id=") << session_id;
  response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  response.extract_json().get().as_array();
  EXPECT_GT(chat_list.size(), static_cast<size_t>(0));

  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_room=") << "abc"
      << UU("&session_id=") << session_id;
  response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  chat_list = response.extract_json().get().as_array();
  EXPECT_GT(chat_list.size(), static_cast<size_t>(0));
}

TEST_F(ChatServerTest, Get_ChatMessage_Fail_Invalid_RoomName) {
  const string_t session_id = PerformSuccessfulLogin();
  // Test for invalid room name
  ostringstream_t buf;
  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_room=") << "invalid_room"
      << UU("&session_id=") << session_id;
  http_response response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
}

TEST_F(ChatServerTest, Get_ChatMessage_Fail_Missing_RoomName) {
  const string_t session_id = PerformSuccessfulLogin();
  // Test for missing room name
  ostringstream_t buf;
  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?session_id=") << session_id;
  http_response response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
}

TEST_F(ChatServerTest, Get_ChatMessage_Fail_Invalid_SessionId) {
  const string_t session_id = PerformSuccessfulLogin();
  // Test for invalid session id
  ostringstream_t buf;
  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_room=") << "abc"
      << UU("&session_id=") << "invalid";
  http_response response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::Forbidden);
  EXPECT_EQ(body, UU("Not a valid session ID"));
}

TEST_F(ChatServerTest, Get_ChatMessage_Fail_Missing_SessionId) {
  const string_t session_id = PerformSuccessfulLogin();
  // Test for missing session id
  ostringstream_t buf;
  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_room=") << "abc";
  http_response response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  string_t body = 
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::Forbidden);
  EXPECT_EQ(body, UU("Not a valid session ID"));
}

TEST_F(ChatServerTest, Get_ChatRoom_Success) {
  const string_t session_id = PerformSuccessfulLogin();
  // Test for success to get chat rooms
  ostringstream_t buf;
  buf.str(UU(""));
  buf.clear();
  buf << "chatroom" << UU("?session_id=") << session_id;
  http_response response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  json::array chat_list = response.extract_json().get().as_array();
  EXPECT_GT(chat_list.size(), static_cast<size_t>(0));
}

TEST_F(ChatServerTest, Get_ChatRoom_Fail_InvalidSessionId) {
  const string_t session_id = PerformSuccessfulLogin();
  // Test for invalid session id
  ostringstream_t buf;
  buf.str(UU(""));
  buf.clear();
  buf << "chatroom" << UU("?session_id=") << "invalid";
  http_response response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::Forbidden);
  EXPECT_EQ(body, UU("Not a valid session ID"));
}

TEST_F(ChatServerTest, Get_ChatRoom_Fail_Missing_SessionId) {
  const string_t session_id = PerformSuccessfulLogin();
  // Test for missing session id
  ostringstream_t buf;
  buf.str(UU(""));
  buf.clear();
  buf << "chatroom";
  http_response response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  string_t body = 
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::Forbidden);
  EXPECT_EQ(body, UU("Not a valid session ID"));
}