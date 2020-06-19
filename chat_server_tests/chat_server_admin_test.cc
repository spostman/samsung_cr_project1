// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#include "chat_server.h"
#include "gtest/gtest.h"
#include "cpprest/http_client.h"
#include "chat_server_admin_test_fixture.h"

using namespace std;
using namespace utility;
using namespace chatserver;
using namespace chatservertests;
using namespace web;
using ::concurrency::task_status;
using ::web::http::client::http_client;
using ::web::http::http_response;
using ::web::json::value;

TEST_F(ChatServerAdminTest, Post_CreateAccount_Success) {
  ostringstream_t buf;
  buf << UU("account");
  value body_data;
  body_data[UU("id")] = value::string(UU("wsplab"));
  body_data[UU("password")] = value::string(HashString(UU("bestkaistlab")));
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}

TEST_F(ChatServerAdminTest, Post_CreateAccount_Success_WithNumbers) {
  ostringstream_t buf;
  buf << UU("account");
  value body_data;
  body_data[UU("id")] = value::string(UU("wsp123"));
  body_data[UU("password")] = value::string(HashString(UU("bestkaistlab")));
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}

TEST_F(ChatServerAdminTest, Post_CreateAccount_Fail_DuplicateId) {
  // Test for duplicate ID.
  ostringstream_t buf;
  buf << UU("account");
  value body_data;
  body_data[UU("id")] = value::string(UU("samsung"));
  body_data[UU("password")] = value::string(HashString(UU("12345678")));
  http_response response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str()), body_data).get();
  string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
}

TEST_F(ChatServerAdminTest, Post_Login_Success) {
  // Test for success to login.
  ostringstream_t buf;
  buf << UU("login");
  value body_data;
  string_t nonce = GenerateNonce();
  body_data[UU("id")] = value::string(UU("samsung"));
  body_data[UU("nonce")] = value::string(nonce);
  body_data[UU("password")] =
      value::string(HashLoginPassword(UU("12345678"), nonce));
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}

TEST_F(ChatServerAdminTest, Post_Login_Fail_IdNotExist) {
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
}

TEST_F(ChatServerAdminTest, Post_Login_Fail_PasswordError) {
  // Test for password error.
  ostringstream_t buf;
  buf << UU("login");
  value body_data;
  string_t nonce = GenerateNonce();
  body_data[UU("id")] = value::string(UU("samsung"));
  body_data[UU("nonce")] = value::string(nonce);
  body_data[UU("password")] =
      value::string(HashLoginPassword(UU("12345678489ffwe"), nonce));
  http_response response =
      http_client_->request(http::methods::POST,
          uri::encode_uri(buf.str()), body_data).get();
  string_t body =
      response.content_ready().get().extract_utf16string(true).get();
  EXPECT_EQ(response.status_code(), http::status_codes::BadRequest);
}

TEST_F(ChatServerAdminTest, Post_InputChatMessage_Success) {
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
}

TEST_F(ChatServerAdminTest, Post_CreateChatRoom_Success) {
  string_t session_id = PerformSuccessfulLogin();
  // Test for success to make a chat room.
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

TEST_F(ChatServerAdminTest, Post_CreateChatRoom_Fail_Duplicate_ChatRoom) {
  string_t session_id = PerformSuccessfulLogin();
  // Test for making duplicated chat room.
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

TEST_F(ChatServerAdminTest, Get_ChatMessage_Success) {
  const string_t session_id = PerformSuccessfulLogin();
  // Test for getting chat messages for each chat rooms.
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
}

TEST_F(ChatServerAdminTest, Get_ChatRoom_Success) {
  const string_t session_id = PerformSuccessfulLogin();
  // Test for success to get chat rooms.
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

TEST_F(ChatServerAdminTest, Delete_Logout_Success) {
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

TEST_F(ChatServerAdminTest, EndToEndTest_OneClient) {
  // Success to create an account.
  ostringstream_t buf;
  buf << UU("account");
  value body_data;
  body_data[UU("id")] = value::string(UU("wsplab"));
  body_data[UU("password")] = value::string(HashString(UU("bestkaistlab")));
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);

  // Success to login.
  buf.str(UU(""));
  buf << UU("login");
  string_t nonce = GenerateNonce();
  body_data[UU("id")] = value::string(UU("wsplab"));
  body_data[UU("nonce")] = value::string(nonce);
  body_data[UU("password")] =
      value::string(HashLoginPassword(UU("bestkaistlab"), nonce));
  response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str()), body_data).get();
  value object = response.extract_json().get();
  string_t session_id = object[UU("session_id")].as_string();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);

  // Get chat room list.
  buf.str(UU(""));
  buf << "chatroom" << UU("?session_id=") << session_id;
  response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  json::array chat_room_list = response.extract_json().get().as_array();
  size_t chat_room_size = chat_room_list.size();
  EXPECT_GT(chat_room_size, static_cast<size_t>(0));

  // Test for success to make a chat room.
  buf.str(UU(""));
  buf << UU("chatroom");
  body_data[UU("chat_room")] = value::string(UU("12345"));
  body_data[UU("session_id")] = value::string(session_id);
  response = http_client_->request(http::methods::POST,
                                   uri::encode_uri(buf.str()), 
                                   body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);

  // Check number of chat room.
  buf.str(UU(""));
  buf << "chatroom" << UU("?session_id=") << session_id;
  response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  chat_room_list = response.extract_json().get().as_array();
  bool is_exist = false;
  // Check if the created chat room exists.
  for (auto& i : chat_room_list) {
    if (i[UU("room")].as_string().find_first_of(UU("12345")) != 
        string_t::npos) {
      is_exist = true;
      break;
    }
  }
  EXPECT_EQ(chat_room_size + 1, chat_room_list.size());

  // Test for getting chat messages.
  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_room=") << "1"
      << UU("&session_id=") << session_id;
  response = http_client_->request(http::methods::GET,
                                   uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  json::array chat_list = response.extract_json().get().as_array();
  size_t chat_list_size = chat_list.size();
  EXPECT_GT(chat_list_size, static_cast<size_t>(0));

  // Success to input a chat message.
  buf.str(UU(""));
  buf << UU("chatmessage");
  body_data[UU("chat_message")] = value::string(UU("good day~!"));
  body_data[UU("chat_room")] = value::string(UU("1"));
  body_data[UU("session_id")] = value::string(session_id);
  response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);

  // Getting chat messages to check successful chat message input.
  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_room=") << "1"
      << UU("&session_id=") << session_id;
  response = http_client_->request(http::methods::GET,
                                   uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  chat_list = response.extract_json().get().as_array();
  is_exist = false;
  // Check if the entered message exists.
  for (auto& i : chat_list) {
    if (i[UU("message")].as_string().find_first_of(UU("good day~!")) != 
        string_t::npos) {
      is_exist = true;
      break;
    }
  }
  EXPECT_EQ(chat_list_size + 1, chat_list.size());
  EXPECT_EQ(true, is_exist);

  // Test success to logout.
  buf.str(UU(""));
  buf.clear();
  buf << "session" << UU("?session_id=") << session_id;
  response = http_client_->request(http::methods::DEL,
                                   uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}

TEST_F(ChatServerAdminTest, EndToEndTest_TwoClient) {
  // Success to login samsung.
  ostringstream_t buf;
  buf.str(UU(""));
  buf << UU("login");
  string_t nonce = GenerateNonce();
  value body_data;
  body_data[UU("id")] = value::string(UU("samsung"));
  body_data[UU("nonce")] = value::string(nonce);
  body_data[UU("password")] =
      value::string(HashLoginPassword(UU("12345678"), nonce));
  http_response response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str()), body_data).get();
  value object = response.extract_json().get();
  string_t session_id_samsung = object[UU("session_id")].as_string();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);

  // Success to login galaxy.
  buf.str(UU(""));
  buf << UU("login");
  nonce = GenerateNonce();
  body_data[UU("id")] = value::string(UU("galaxy"));
  body_data[UU("nonce")] = value::string(nonce);
  body_data[UU("password")] =
      value::string(HashLoginPassword(UU("abcdefgh"), nonce));
  response = http_client_->request(
      http::methods::POST, uri::encode_uri(buf.str()), body_data).get();
  object = response.extract_json().get();
  string_t session_id_galaxy = object[UU("session_id")].as_string();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);

  // Get chat room list.
  buf.str(UU(""));
  buf << "chatroom" << UU("?session_id=") << session_id_samsung;
  response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  json::array chat_room_list = response.extract_json().get().as_array();
  size_t chat_room_size = chat_room_list.size();
  EXPECT_GT(chat_room_size, static_cast<size_t>(0));

  // Test for success to make a chat room.
  buf.str(UU(""));
  buf << UU("chatroom");
  body_data[UU("chat_room")] = value::string(UU("12345"));
  body_data[UU("session_id")] = value::string(session_id_galaxy);
  response = http_client_->request(http::methods::POST,
                                   uri::encode_uri(buf.str()),
                                   body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);

  // Check number of chat room.
  buf.str(UU(""));
  buf << "chatroom" << UU("?session_id=") << session_id_samsung;
  response = http_client_->request(http::methods::GET,
                                   uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  chat_room_list = response.extract_json().get().as_array();
  EXPECT_EQ(chat_room_size + 1, chat_room_list.size());

  // Test for getting chat messages.
  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_room=") << "1"
      << UU("&session_id=") << session_id_galaxy;
  response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  chat_room_list = response.extract_json().get().as_array();
  size_t chat_list_size = chat_room_list.size();
  EXPECT_GT(chat_list_size, static_cast<size_t>(0));

  // Success to input a chat message by samsung.
  buf.str(UU(""));
  buf << UU("chatmessage");
  body_data[UU("chat_message")] = value::string(UU("good day~!"));
  body_data[UU("chat_room")] = value::string(UU("1"));
  body_data[UU("session_id")] = value::string(session_id_samsung);
  response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);

  // Getting chat messages to check successful chat message input.
  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_room=") << "1"
      << UU("&session_id=") << session_id_galaxy;
  response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  json::array chat_list = response.extract_json().get().as_array();
  bool is_exist = false;
  // Check if the entered message exists.
  for (auto& i : chat_list) {
    if(i[UU("message")].as_string().find_first_of(UU("good day~!")) != 
       string_t::npos) {
      is_exist = true;
      break;
    }
  }
  EXPECT_EQ(chat_list_size + 1, chat_list.size());
  EXPECT_EQ(true, is_exist);

  // Success to input a chat message by galaxy.
  buf.str(UU(""));
  buf << UU("chatmessage");
  body_data[UU("chat_message")] = value::string(UU("Sunny day!"));
  body_data[UU("chat_room")] = value::string(UU("1"));
  body_data[UU("session_id")] = value::string(session_id_galaxy);
  response = http_client_->request(http::methods::POST,
      uri::encode_uri(buf.str()), body_data).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);

  // Getting chat messages to check successful chat message input.
  buf.str(UU(""));
  buf.clear();
  buf << "chatmessage" << UU("?chat_room=") << "1"
      << UU("&session_id=") << session_id_samsung;
  response = http_client_->request(http::methods::GET,
      uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
  chat_list = response.extract_json().get().as_array();
  is_exist = false;
  // Check if the entered message exists.
  for (auto& i : chat_list) {
    if (i[UU("message")].as_string().find_first_of(UU("Sunny day!")) != 
        string_t::npos) {
      is_exist = true;
    }
  }
  EXPECT_EQ(chat_list_size + 2, chat_list.size());
  EXPECT_EQ(true, is_exist);

  // Test success to logout.
  buf.str(UU(""));
  buf << "session" << UU("?session_id=") << session_id_galaxy;
  response = http_client_->request(http::methods::DEL,
                                   uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);

  buf.str(UU(""));
  buf << "session" << UU("?session_id=") << session_id_samsung;
  response = http_client_->request(http::methods::DEL,
                                   uri::encode_uri(buf.str())).get();
  EXPECT_EQ(response.status_code(), http::status_codes::OK);
}