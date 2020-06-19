// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#include "chat_client.h"

#include <cstdlib>
#include <stdio.h>

#include "chat_client_admin_test_fixture.h"
#include "gtest/gtest.h"

using namespace std;
using namespace utility;
using namespace chatclient;
using namespace std::chrono;

TEST_F(ChatClientAdminTest, SignupTest_Success) {
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

TEST_F(ChatClientAdminTest, LoginTest_Success) {
  FILE *f1 = nullptr;
  // Login success.
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("login\n", f1);
  fputs("samsung\n", f1);
  fputs("12345678\n", f1);
  fputs("exit\n", f1);
  fclose(f1);
  freopen_s(&f1, "test_input.txt", "r", stdin);
  chat_client_ = GetChatClient();
  string_t standard_output = GetStandardOutputFromChatClient(chat_client_);
  fclose(f1);
  EXPECT_NE(string_t::npos, standard_output.find(UU("Succeed to login")));
}

TEST_F(ChatClientAdminTest, DisplayChatRoomTest) {
  FILE *f1 = nullptr;
  // Success to display chat room list.
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("login\n", f1);
  fputs("samsung\n", f1);
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

TEST_F(ChatClientAdminTest, CreateChatRoomTest_Success) {
  FILE *f1 = nullptr;
  // Success to create chat room.
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("login\n", f1);
  fputs("samsung\n", f1);
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

TEST_F(ChatClientAdminTest, LogoutTest) {
  // Logout success.
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  FILE *f1 = nullptr;
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("login\n", f1);
  fputs("samsung\n", f1);
  fputs("12345678\n", f1);
  fputs("logout\n", f1);
  fputs("exit\n", f1);
  fclose(f1);
  freopen_s(&f1, "test_input.txt", "r", stdin);
  chat_client_ = GetChatClient();
  string_t standard_output = GetStandardOutputFromChatClient(chat_client_);
  fclose(f1);
  EXPECT_NE(string_t::npos, standard_output.find(UU("Logout succeeds")));
}

TEST_F(ChatClientAdminTest, ChatRoomInputMessageTest_Success) {
  FILE *f1 = nullptr;
  // Input chat message.
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("login\n", f1);
  fputs("samsung\n", f1);
  fputs("12345678\n", f1);
  fputs("join\n", f1);
  fputs("1\n", f1);
  fputs("test\n", f1);
  fputs("quit\n", f1);
  fputs("join\n", f1);
  fputs("1\n", f1);
  fputs("test\n", f1);
  fputs("quit\n", f1);
  fputs("exit\n", f1);
  fclose(f1);
  freopen_s(&f1, "test_input.txt", "r", stdin);
  chat_client_ = GetChatClient();
  string_t standard_output =
      GetStandardOutputFromChatClientWithSleep(chat_client_,
                                               milliseconds(1500));
  fclose(f1);
  EXPECT_NE(string_t::npos,
      standard_output.find(UU("Chat messages in [1]")));
  EXPECT_NE(string_t::npos,
      standard_output.find(UU("You are in [1]")));
  EXPECT_NE(string_t::npos,
      standard_output.find(UU("Input chat message")));
  EXPECT_NE(string_t::npos,
      standard_output.find(UU("<samsung> test")));
}


TEST_F(ChatClientAdminTest, ChatRoomGetMessageTest_Success) {
  FILE *f1 = nullptr;
  // Input chat message to the chat room of "1"
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("login\n", f1);
  fputs("samsung\n", f1);
  fputs("12345678\n", f1);
  fputs("join\n", f1);
  fputs("1\n", f1);
  fputs("hihi\n", f1);
  fputs("quit\n", f1);
  fputs("exit\n", f1);
  fclose(f1);
  freopen_s(&f1, "test_input.txt", "r", stdin);
  chat_client_ = GetChatClient();
  string_t standard_output =
      GetStandardOutputFromChatClientWithSleep(chat_client_,
                                               milliseconds(1500));
  fclose(f1);
  EXPECT_NE(string_t::npos,
      standard_output.find(UU("Chat messages in [1]")));
  EXPECT_NE(string_t::npos,
      standard_output.find(UU("[2020-03-02 16:42:10] <samsung> hello")));
}

TEST_F(ChatClientAdminTest, EndToEndTest) {
  // Test every function at once.
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  FILE *f1 = nullptr;
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("signup\n", f1);
  fputs("test\n", f1);
  fputs("12345678\n", f1);
  fputs("12345678\n", f1);
  fputs("login\n", f1);
  fputs("test\n", f1);
  fputs("12345678\n", f1);
  fputs("room_create\n", f1);
  fputs("test\n", f1);
  fputs("room_list\n", f1);
  fputs("join\n", f1);
  fputs("1\n", f1);
  fputs("hello! i'm test\n", f1);
  fputs("quit\n", f1);
  fputs("join\n", f1);
  fputs("1\n", f1);
  fputs("test\n", f1);
  fputs("quit\n", f1);
  fputs("exit\n", f1);
  fclose(f1);
  freopen_s(&f1, "test_input.txt", "r", stdin);
  chat_client_ = GetChatClient();
  string_t standard_output =
      GetStandardOutputFromChatClientWithSleep(chat_client_,
                                               milliseconds(3000));
  fclose(f1);
  EXPECT_NE(string_t::npos,
      standard_output.find(UU("Chat messages in [1]")));
  EXPECT_NE(string_t::npos,
      standard_output.find(UU("<samsung> hello!!! hihi")));
  EXPECT_NE(string_t::npos,
      standard_output.find(UU("<test> hello! i'm test")));
  EXPECT_NE(string_t::npos,
      standard_output.find(UU("quit to leave the chat room")));
}