// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#include "chat_client.h"

#include <cstdlib>
#include <stdio.h>

#include "chat_client_test_fixture.h"
#include "gtest/gtest.h"

using namespace std;
using namespace utility;
using namespace chatclient;

TEST_F(ChatClientTest, SignupTest_Success) {
  FILE *f1 = nullptr;
  // Signup success.
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("signup\n", f1);
  fputs("test\n", f1);
  fputs("12345678\n", f1);
  fputs("12345678\n", f1);
  fputs("exit\n", f1);
  fclose(f1);
  freopen_s(&f1, "test_input.txt", "r", stdin);
  chat_client_ = GetChatClient();
  string_t standard_output = GetStandardOutputFromChatClient(chat_client_);
  fclose(f1);
  EXPECT_NE(string_t::npos, standard_output.find(UU("Success to signup")));
}

TEST_F(ChatClientTest, SignupTest_Fail_DuplicatedId) {
  FILE *f1 = nullptr;
  // Signup fails due to duplicated ID.
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("signup\n", f1);
  fputs("kaist\n", f1);
  fputs("12312345678\n", f1);
  fputs("12312345678\n", f1);
  fputs("exit\n", f1);
  fclose(f1);
  freopen_s(&f1, "test_input.txt", "r", stdin);
  chat_client_ = GetChatClient();
  string_t standard_output = GetStandardOutputFromChatClient(chat_client_);
  fclose(f1);
  EXPECT_NE(string_t::npos, standard_output.find(UU("HTTP error response")));
  EXPECT_NE(string_t::npos, standard_output.find(UU("Duplicated ID")));
}

TEST_F(ChatClientTest, SignupTest_Fail_ProhibitedCharId) {
  FILE *f1 = nullptr;
  // Signup fails due to prohibited character in ID.
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("signup\n", f1);
  fputs("ka,ist\n", f1);
  fputs("12312345678\n", f1);
  fputs("12312345678\n", f1);
  fputs("exit\n", f1);
  fclose(f1);
  freopen_s(&f1, "test_input.txt", "r", stdin);
  chat_client_ = GetChatClient();
  string_t standard_output = GetStandardOutputFromChatClient(chat_client_);
  fclose(f1);
  EXPECT_NE(string_t::npos, standard_output.find(UU("HTTP error response")));
  EXPECT_NE(string_t::npos,
      standard_output.find(UU("Prohibited character in ID")));
}

TEST_F(ChatClientTest, SignupTest_Fail_Password) {
  FILE *f1 = nullptr;
  // Signup fails due to password not matching.
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("signup\n", f1);
  fputs("test\n", f1);
  fputs("123456789\n", f1);
  fputs("12345678\n", f1);
  fputs("exit\n", f1);
  fclose(f1);
  freopen_s(&f1, "test_input.txt", "r", stdin);
  chat_client_ = GetChatClient();
  string_t standard_output = GetStandardOutputFromChatClient(chat_client_);
  fclose(f1);
  EXPECT_NE(string_t::npos, standard_output.find(UU("Password not matching")));
}

TEST_F(ChatClientTest, SignupTest_Fail_ShortLength_Password) {
  FILE *f1 = nullptr;
  // Signup fails due to a short length password.
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("signup\n", f1);
  fputs("test\n", f1);
  fputs("1234\n", f1);
  fputs("1234\n", f1);
  fputs("exit\n", f1);
  fclose(f1);
  freopen_s(&f1, "test_input.txt", "r", stdin);
  chat_client_ = GetChatClient();
  string_t standard_output = GetStandardOutputFromChatClient(chat_client_);
  fclose(f1);
  EXPECT_NE(string_t::npos, standard_output.find(UU("Password must be at least 8 characters")));
}

TEST_F(ChatClientTest, LoginTest_Success) {
  FILE *f1 = nullptr;
  // Login success.
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("login\n", f1);
  fputs("kaist\n", f1);
  fputs("12345678\n", f1);
  fputs("exit\n", f1);
  fclose(f1);
  freopen_s(&f1, "test_input.txt", "r", stdin);
  chat_client_ = GetChatClient();
  string_t standard_output = GetStandardOutputFromChatClient(chat_client_);
  fclose(f1);
  EXPECT_NE(string_t::npos, standard_output.find(UU("Success to login")));
  EXPECT_EQ(GetSessionID().length(), 32);
  EXPECT_EQ(UU("kaist"), GetUserID());
  EXPECT_EQ(ChatClient::kAfterLogin, GetClientCurrentStatus());
}

TEST_F(ChatClientTest, LoginTest_Fail_IncorrectId) {
  FILE *f1 = nullptr;
  // Login fail with incorrect ID.
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("login\n", f1);
  fputs("kaistt\n", f1);
  fputs("12345678\n", f1);
  fputs("exit\n", f1);
  fclose(f1);
  freopen_s(&f1, "test_input.txt", "r", stdin);
  chat_client_ = GetChatClient();
  string_t standard_output = GetStandardOutputFromChatClient(chat_client_);
  fclose(f1);
  EXPECT_NE(string_t::npos, standard_output.find(UU("HTTP error response")));
  EXPECT_NE(string_t::npos, standard_output.find(UU("ID not exist")));
  EXPECT_EQ(GetSessionID().length(), 0);
  EXPECT_EQ(UU(""), GetUserID());
  EXPECT_EQ(ChatClient::kBeforeLogin, GetClientCurrentStatus());
}

TEST_F(ChatClientTest, LoginTest_Fail_IncorrectPassword) {
  FILE *f1 = nullptr;
  // Login fail with incorrect password.
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("login\n", f1);
  fputs("kaist\n", f1);
  fputs("1234555678\n", f1);
  fputs("exit\n", f1);
  fclose(f1);
  freopen_s(&f1, "test_input.txt", "r", stdin);
  chat_client_ = GetChatClient();
  string_t standard_output = GetStandardOutputFromChatClient(chat_client_);
  fclose(f1);
  EXPECT_NE(string_t::npos, standard_output.find(UU("HTTP error response")));
  EXPECT_NE(string_t::npos, standard_output.find(UU("Password error")));
  EXPECT_EQ(GetSessionID().length(), 0);
  EXPECT_EQ(UU(""), GetUserID());
  EXPECT_EQ(ChatClient::kBeforeLogin, GetClientCurrentStatus());
}

TEST_F(ChatClientTest, DisplayChatRoomTest) {
  FILE *f1 = nullptr;
  // Success to display chat room list.
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("login\n", f1);
  fputs("kaist\n", f1);
  fputs("12345678\n", f1);
  fputs("room_list\n", f1);
  fputs("exit\n", f1);
  fclose(f1);
  freopen_s(&f1, "test_input.txt", "r", stdin);
  chat_client_ = GetChatClient();
  string_t standard_output = GetStandardOutputFromChatClient(chat_client_);
  fclose(f1);
  EXPECT_NE(string_t::npos, standard_output.find(UU("chat room list")));
}

TEST_F(ChatClientTest, CreateChatRoomTest_Success) {
  FILE *f1 = nullptr;
  // Success to create chat room.
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("login\n", f1);
  fputs("kaist\n", f1);
  fputs("12345678\n", f1);
  fputs("room_create\n", f1);
  fputs("test\n", f1);
  fputs("exit\n", f1);
  fclose(f1);
  freopen_s(&f1, "test_input.txt", "r", stdin);
  chat_client_ = GetChatClient();
  string_t standard_output = GetStandardOutputFromChatClient(chat_client_);
  fclose(f1);
  EXPECT_NE(string_t::npos, standard_output.find(UU("Create chat room")));
}

TEST_F(ChatClientTest, CreateChatRoomTest_Fail_DuplicateName) {
  FILE *f1 = nullptr;
  // Fail to create chat room due to the chat room name already exists.
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("login\n", f1);
  fputs("kaist\n", f1);
  fputs("12345678\n", f1);
  fputs("room_create\n", f1);
  fputs("abc\n", f1);
  fputs("exit\n", f1);
  fclose(f1);
  freopen_s(&f1, "test_input.txt", "r", stdin);
  chat_client_ = GetChatClient();
  string_t standard_output = GetStandardOutputFromChatClient(chat_client_);
  fclose(f1);
  EXPECT_NE(string_t::npos, 
            standard_output.find(UU("Given chat room: abc already exists")));
}

TEST_F(ChatClientTest, CreateChatRoomTest_Fail_ProhibitedChar) {
  FILE *f1 = nullptr;
  // Fail to create chat room due to the prohibited char.
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("login\n", f1);
  fputs("kaist\n", f1);
  fputs("12345678\n", f1);
  fputs("room_create\n", f1);
  fputs("abc|\n", f1);
  fputs("exit\n", f1);
  fclose(f1);
  freopen_s(&f1, "test_input.txt", "r", stdin);
  chat_client_ = GetChatClient();
  string_t standard_output = GetStandardOutputFromChatClient(chat_client_);
  fclose(f1);
  EXPECT_NE(string_t::npos, 
            standard_output.find(UU("Prohibited char in the chat room")));
}

TEST_F(ChatClientTest, LogoutTest) {
  // Logout success.
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  FILE *f1 = nullptr;
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("login\n", f1);
  fputs("kaist\n", f1);
  fputs("12345678\n", f1);
  fputs("logout\n", f1);
  fputs("exit\n", f1);
  fclose(f1);
  freopen_s(&f1, "test_input.txt", "r", stdin);
  chat_client_ = GetChatClient();
  string_t standard_output = GetStandardOutputFromChatClient(chat_client_);
  fclose(f1);
  EXPECT_NE(string_t::npos, standard_output.find(UU("Logout succeeds")));
  EXPECT_EQ(GetSessionID().length(), 0);
  EXPECT_EQ(UU(""), GetUserID());
  EXPECT_EQ(ChatClient::kBeforeLogin, GetClientCurrentStatus());
  EXPECT_EQ(UU(""), GetCurrentChatRoom());
}