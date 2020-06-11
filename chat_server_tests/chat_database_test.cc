// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#include "gtest/gtest.h"
#include "chat_database.h"
#include "chat_message.h"

using namespace std;
using namespace utility;
using namespace chatserver;

// Fixture class for chat_database.h testing.
class ChatDatabaseTest : public ::testing::Test {
 protected:
  ChatDatabase chat_database_;
  // Delimiter in the chat message file database.
  string_t kParsingDelimeterChatDb = UU("|");

  void SetUp() override {
    const string_t chat_message_file = UU("chat_messages.txt");
    const string_t chat_room_file = UU("chat_room.txt");
    wofstream file(chat_message_file, wofstream::out | ofstream::trunc);
    file << "1583581783|kaist|a|hihi" << endl;
    file << "1583581784|wsp|a|hello" << endl;
    file << "1583581785|kaist|b|hello world" << endl;
    file << "1583581786|wsp|c|??" << endl;
    file.close();

    file.open(chat_room_file, wofstream::out | ofstream::trunc);
    file << "a" << endl;
    file << "b" << endl;
    file << "c" << endl;
    file.close();
    chat_database_.Initialize(chat_message_file, chat_room_file);
  }
};

TEST_F(ChatDatabaseTest, Initialization_Fail) {
  // Check initialization fails.
  EXPECT_EQ(false, chat_database_.Initialize(UU("hello.txt"), UU("hi.txt")));
  EXPECT_EQ(
    false, chat_database_.Initialize(UU("chat_messages.txt"), UU("hi.txt")));
  EXPECT_EQ(
    false, chat_database_.Initialize(UU("abcdef.txt"), UU("chat_room.txt")));
}

TEST_F(ChatDatabaseTest, GetAllChatMessages) {
  // Check number of messages from the chat room
  vector<ChatMessage> chat_message;
  EXPECT_EQ(2, chat_database_.GetAllChatMessages(UU("a"))->size());
  EXPECT_EQ(1, chat_database_.GetAllChatMessages(UU("b"))->size());
  EXPECT_EQ(1, chat_database_.GetAllChatMessages(UU("c"))->size());
  EXPECT_EQ(0, chat_database_.GetAllChatMessages(UU("d"))->size());
  EXPECT_EQ(0, chat_database_.GetAllChatMessages(UU("e"))->size());
}

TEST_F(ChatDatabaseTest, StoreChatMessage_Success) {
  // Success to store message.
  ChatMessage message;
  message.date = 1583581800;
  message.user_id = UU("gsis");
  message.chat_room = UU("c");
  message.chat_message = UU("haha");
  EXPECT_EQ(true, chat_database_.StoreChatMessage(message));
  EXPECT_EQ(2, chat_database_.GetAllChatMessages(UU("c"))->size());
}

TEST_F(ChatDatabaseTest, StoreChatMessage_Fail_ProhibitedChar_Id) {
  // Fail to store message due to prohibited char.
  ChatMessage message;
  message.date = 1583581800;
  message.user_id = UU("gsis" + kParsingDelimeterChatDb);
  message.chat_room = UU("c");
  message.chat_message = UU("haha");
  EXPECT_EQ(false, chat_database_.StoreChatMessage(message));
  EXPECT_EQ(1, chat_database_.GetAllChatMessages(UU("c"))->size());
}

TEST_F(ChatDatabaseTest, StoreChatMessage_Fail_ProhibitedChar_Room) {
  // Fail to store message due to prohibited char.
  ChatMessage message;
  message.date = 1583581800;
  message.user_id = UU("gsis");
  message.chat_room = UU("c" + kParsingDelimeterChatDb);
  message.chat_message = UU("haha");
  EXPECT_EQ(false, chat_database_.StoreChatMessage(message));
  EXPECT_EQ(1, chat_database_.GetAllChatMessages(UU("c"))->size());
}

TEST_F(ChatDatabaseTest, StoreChatMessage_Fail_ProhibitedChar_Message) {
  // Fail to store message due to prohibited char.
  ChatMessage message;
  message.date = 1583581800;
  message.user_id = UU("gsis");
  message.chat_room = UU("c");
  message.chat_message = UU("haha" + kParsingDelimeterChatDb);
  EXPECT_EQ(false, chat_database_.StoreChatMessage(message));
  EXPECT_EQ(1, chat_database_.GetAllChatMessages(UU("c"))->size());
}

TEST_F(ChatDatabaseTest, GetChatRoomList) {
  EXPECT_EQ(3, chat_database_.GetChatRoomList()->size());
}

TEST_F(ChatDatabaseTest, CreateChatRoom_Success) {
  chat_database_.CreateChatRoom(UU("d"));
  EXPECT_EQ(4, chat_database_.GetChatRoomList()->size());
}

TEST_F(ChatDatabaseTest, CreateChatRoom_Fail_Duplicate) {
  // Check duplicated chat name.
  chat_database_.CreateChatRoom(UU("a"));
  EXPECT_EQ(3, chat_database_.GetChatRoomList()->size());
}

TEST_F(ChatDatabaseTest, CreateChatRoom_Fail_InvalidName) {
  // Check invalid chat name.
  chat_database_.CreateChatRoom(UU(""));
  EXPECT_EQ(3, chat_database_.GetChatRoomList()->size());
}

TEST_F(ChatDatabaseTest, DoesDelimiterExistInChatMessage_Success) {
  ChatMessage message;
  message.date = 1583581800;
  message.user_id = UU("gsis");
  message.chat_room = UU("c");
  message.chat_message = UU("haha");
  EXPECT_EQ(true, chat_database_.DoesDelimiterExistInChatMessage(message));
}

TEST_F(ChatDatabaseTest, DoesDelimiterExist_Fail_DelimiterInId) {
  ChatMessage message;
  message.date = 1583581800;
  message.user_id = UU("gsis" + kParsingDelimeterChatDb);
  message.chat_room = UU("c");
  message.chat_message = UU("haha");
  EXPECT_EQ(false, chat_database_.DoesDelimiterExistInChatMessage(message));
}

TEST_F(ChatDatabaseTest, DoesDelimiterExist_Fail_DelimiterInRoom) {
  ChatMessage message;
  message.date = 1583581800;
  message.user_id = UU("gsis");
  message.chat_room = UU("c" + kParsingDelimeterChatDb);
  message.chat_message = UU("haha");
  EXPECT_EQ(false, chat_database_.DoesDelimiterExistInChatMessage(message));
}

TEST_F(ChatDatabaseTest, DoesDelimiterExist_Fail_DelimiterInMessage) {
  ChatMessage message;
  message.date = 1583581800;
  message.user_id = UU("gsis");
  message.chat_room = UU("c");
  message.chat_message = UU("haha" + kParsingDelimeterChatDb);
  EXPECT_EQ(false, chat_database_.DoesDelimiterExistInChatMessage(message));
}

TEST(ChatDatabase, ParsingChatMessages_Success) {
  ChatDatabase chat_database;
  const string_t chat_message_file = UU("chat_messages.txt");
  const string_t chat_room_file = UU("chat_room.txt");

  wofstream file(chat_message_file, wofstream::out | ofstream::trunc);
  file << "1583581783|kaist|a|hihi" << endl;
  file.close();

  // Success to parse chat message.
  file.open(chat_room_file, wofstream::out | ofstream::trunc);
  file << "a" << endl;
  file.close();
  EXPECT_EQ(true, chat_database.Initialize(chat_message_file, chat_room_file));
}

TEST(ChatDatabase, ParsingChatMessages_Fail_One_Delimiter) {
  ChatDatabase chat_database;
  const string_t chat_message_file = UU("chat_messages.txt");
  const string_t chat_room_file = UU("chat_room.txt");

  wofstream file(chat_message_file, wofstream::out | ofstream::trunc);
  file << "1583581783kaist|aihi" << endl;
  file.close();
  EXPECT_EQ(false, chat_database.Initialize(chat_message_file, chat_room_file));
}
TEST(ChatDatabase, ParsingChatMessages_Fail_Two_Delimiters) {
  ChatDatabase chat_database;
  const string_t chat_message_file = UU("chat_messages.txt");
  const string_t chat_room_file = UU("chat_room.txt");

  wofstream file(chat_message_file, wofstream::out | ofstream::trunc);
  file << "1583581783|kaista|hihi" << endl;
  file.close();
  EXPECT_EQ(false, chat_database.Initialize(chat_message_file, chat_room_file));
}

TEST(ChatDatabase, ParsingChatMessages_Fail_NoData) {
  ChatDatabase chat_database;
  const string_t chat_message_file = UU("chat_messages.txt");
  const string_t chat_room_file = UU("chat_room.txt");

  wofstream file(chat_message_file, wofstream::out | ofstream::trunc);
  file << "|||" << endl;
  file.close();
  EXPECT_EQ(false, chat_database.Initialize(chat_message_file, chat_room_file));
}

TEST(ChatDatabase, ParsingChatMessages_Fail_FirstData) {
  ChatDatabase chat_database;
  const string_t chat_message_file = UU("chat_messages.txt");
  const string_t chat_room_file = UU("chat_room.txt");

  wofstream file(chat_message_file, wofstream::out | ofstream::trunc);
  file << "1583581783|||" << endl;
  file.close();
  EXPECT_EQ(false, chat_database.Initialize(chat_message_file, chat_room_file));
}

TEST(ChatDatabase, ParsingChatMessages_Fail_OnlySecondData) {
  ChatDatabase chat_database;
  const string_t chat_message_file = UU("chat_messages.txt");
  const string_t chat_room_file = UU("chat_room.txt");

  wofstream file(chat_message_file, wofstream::out | ofstream::trunc);
  file << "|kaist||" << endl;
  file.close();
  EXPECT_EQ(false, chat_database.Initialize(chat_message_file, chat_room_file));
}

TEST(ChatDatabase, ParsingChatMessages_Fail_OnlyThirdData) {
  ChatDatabase chat_database;
  const string_t chat_message_file = UU("chat_messages.txt");
  const string_t chat_room_file = UU("chat_room.txt");

  wofstream file(chat_message_file, wofstream::out | ofstream::trunc);
  file << "||a|" << endl;
  file.close();
  EXPECT_EQ(false, chat_database.Initialize(chat_message_file, chat_room_file));
}

TEST(ChatDatabase, ParsingChatMessages_Fail_OnlyLastData) {
  ChatDatabase chat_database;
  const string_t chat_message_file = UU("chat_messages.txt");
  const string_t chat_room_file = UU("chat_room.txt");

  wofstream file(chat_message_file, wofstream::out | ofstream::trunc);
  file << "|||hello" << endl;
  file.close();
  EXPECT_EQ(false, chat_database.Initialize(chat_message_file, chat_room_file));
}

TEST(ChatDatabase, ParsingChatMessages_Fail_Four_Delimiters) {
  ChatDatabase chat_database;
  const string_t chat_message_file = UU("chat_messages.txt");
  const string_t chat_room_file = UU("chat_room.txt");

  wofstream file(chat_message_file, wofstream::out | ofstream::trunc);
  file << "1583581783|kaista|hihi|a|b" << endl;
  file.close();
  EXPECT_EQ(false, chat_database.Initialize(chat_message_file, chat_room_file));
}

TEST(ChatDatabase, ParsingChatMessages_Fail_Five_Delimiters) {
  ChatDatabase chat_database;
  const string_t chat_message_file = UU("chat_messages.txt");
  const string_t chat_room_file = UU("chat_room.txt");

  wofstream file(chat_message_file, wofstream::out | ofstream::trunc);
  file << "1583581783|kaista|hihi|a|b|c" << endl;
  file.close();
  EXPECT_EQ(false, chat_database.Initialize(chat_message_file, chat_room_file));
}

TEST(ChatDatabase, ParsingChatRooms_Success) {
  ChatDatabase chat_database;
  const string_t chat_message_file = UU("chat_messages.txt");
  const string_t chat_room_file = UU("chat_room.txt");

  wofstream file(chat_message_file, wofstream::out | ofstream::trunc);
  file << "1583581783|kaist|a|hihi" << endl;
  file.close();

  file.open(chat_room_file, wofstream::out | ofstream::trunc);
  file << "a" << endl;
  file.close();
  EXPECT_EQ(true, chat_database.Initialize(chat_message_file, chat_room_file));
}
TEST(ChatDatabase, ParsingChatRooms_Success_With_OtherChatRoom) {
  ChatDatabase chat_database;
  const string_t chat_message_file = UU("chat_messages.txt");
  const string_t chat_room_file = UU("chat_room.txt");

  wofstream file(chat_message_file, wofstream::out | ofstream::trunc);
  file << "1583581783|kaist|a|hihi" << endl;
  file.close();

  file.open(chat_room_file, wofstream::out | ofstream::trunc);
  file << "2307" << endl;
  file << "2308" << endl;
  file.close();
  EXPECT_EQ(true, chat_database.Initialize(chat_message_file, chat_room_file));
}

TEST(ChatDatabase, ParsingChatRooms_Fail_Only_ChatRoom) {
  ChatDatabase chat_database;
  const string_t chat_message_file = UU("chat_messages.txt");
  const string_t chat_room_file = UU("chat_room.txt");

  wofstream file(chat_message_file, wofstream::out | ofstream::trunc);
  file << "2307" << endl;
  file << "2307" << endl;
  file.close();
  EXPECT_EQ(false, chat_database.Initialize(chat_message_file, chat_room_file));
}