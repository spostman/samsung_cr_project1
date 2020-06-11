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
using namespace std::chrono;

TEST_F(ChatClientTest, ChatRoomGetMessageTest_Success) {
  FILE *f1 = nullptr;
  // Check chat messages from the chat room of "abc"
  // Input chat message to the chat room of "abc"
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("login\n", f1);
  fputs("kaist\n", f1);
  fputs("12345678\n", f1);
  fputs("join\n", f1);
  fputs("1\n", f1);
  fputs("hihi hihi\n", f1);
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
            standard_output.find(UU("[2020-03-02 16:42:10] <kaist> hello")));

  // Check chat messages from the chat room of "1"
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("login\n", f1);
  fputs("kaist\n", f1);
  fputs("12345678\n", f1);
  fputs("join\n", f1);
  fputs("1\n", f1);
  fputs("quit\n", f1);
  fputs("exit\n", f1);
  fclose(f1);
  freopen_s(&f1, "test_input.txt", "r", stdin);
  chat_client_ = GetChatClient();
  standard_output = 
      GetStandardOutputFromChatClientWithSleep(chat_client_, 
                                               milliseconds(1500));
  fclose(f1);
  EXPECT_NE(string_t::npos, 
            standard_output.find(UU("Chat messages in [1]")));
  EXPECT_NE(string_t::npos, 
            standard_output.find(UU("[2020-03-02 16:42:10] <kaist> hello")));
  EXPECT_NE(
      string_t::npos, 
      standard_output.
          find(UU("[2020-03-02 16:42:20] <gsis> nice to meet you")));
  EXPECT_NE(string_t::npos, standard_output.find(UU("<kaist> hihi hihi")));
}

TEST_F(ChatClientTest, ChatRoomInputMessageTest_Success) {
  FILE *f1 = nullptr;
  // Input chat message to the chat room of "abc"
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("login\n", f1);
  fputs("kaist\n", f1);
  fputs("12345678\n", f1);
  fputs("join\n", f1);
  fputs("abc\n", f1);
  fputs("test1\n", f1);
  fputs("test2\n", f1);
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
      standard_output.find(UU("Chat messages in [abc]")));
  EXPECT_NE(string_t::npos,
      standard_output.find(UU("You are in [abc]")));
  EXPECT_NE(string_t::npos,
      standard_output.find(UU("Input chat message")));

  // Check chat messages input from the above test code
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("login\n", f1);
  fputs("kaist\n", f1);
  fputs("12345678\n", f1);
  fputs("join\n", f1);
  fputs("abc\n", f1);
  fputs("test\n", f1);
  fputs("quit\n", f1);
  fputs("exit\n", f1);
  fclose(f1);
  freopen_s(&f1, "test_input.txt", "r", stdin);
  chat_client_ = GetChatClient();
  standard_output =
      GetStandardOutputFromChatClientWithSleep(chat_client_,
                                               milliseconds(1500));
  fclose(f1);
  EXPECT_NE(string_t::npos,
      standard_output.find(UU("Chat messages in [abc]")));
  EXPECT_NE(string_t::npos,
      standard_output.find(UU("You are in [abc]")));
  EXPECT_NE(string_t::npos,
      standard_output.find(UU("Input chat message")));
  // Testing for input from the above test code.
  EXPECT_NE(string_t::npos, standard_output.find(UU("<kaist> test1")));
  EXPECT_NE(string_t::npos, standard_output.find(UU("<kaist> test2")));
}

TEST_F(ChatClientTest, ChatRoomInputMessageTest_Fail) {
  FILE *f1 = nullptr;
  // Check prohibited char in chat message
  // Mimic user's keyboard input.
  // Enter the contents of the file in C++ standard input.
  EXPECT_EQ(0, fopen_s(&f1, "test_input.txt", "w"));
  fputs("login\n", f1);
  fputs("kaist\n", f1);
  fputs("12345678\n", f1);
  fputs("join\n", f1);
  fputs("abc\n", f1);
  fputs("test|\n", f1);
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
            standard_output.find(UU("Chat messages in [abc]")));
  EXPECT_NE(string_t::npos,
            standard_output.find(UU("You are in [abc]")));
  EXPECT_NE(string_t::npos,
            standard_output.find(UU("Input chat message")));
  EXPECT_NE(string_t::npos,
            standard_output.find(UU("HTTP error response 500:")));
}