#include "gtest/gtest.h"
#include "chat_database.h"
#include "chat_message.h"

using namespace std;
using namespace utility;
using namespace chatserver;

// fixture class for chat_database.h testing
class ChatDatabaseTest : public ::testing::Test {
protected:
  ChatDatabase chat_database_;

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

TEST_F(ChatDatabaseTest, Initialization) {
  EXPECT_EQ(false, chat_database_.Initialize(UU("hello.txt"), UU("hi.txt")));

  EXPECT_EQ(
    false, chat_database_.Initialize(UU("chat_messages.txt"), UU("hi.txt")));

  EXPECT_EQ(
    false, chat_database_.Initialize(UU("abcdef.txt"), UU("chat_room.txt")));
}

TEST_F(ChatDatabaseTest, StoreChatMessage_AND_GetChatList) {
  vector<ChatMessage> chat_message;
  EXPECT_EQ(2, chat_database_.GetAllChatMessages(UU("a")).size());
  EXPECT_EQ(1, chat_database_.GetAllChatMessages(UU("b")).size());
  EXPECT_EQ(1, chat_database_.GetAllChatMessages(UU("c")).size());
  EXPECT_EQ(0, chat_database_.GetAllChatMessages(UU("d")).size());
  EXPECT_EQ(0, chat_database_.GetAllChatMessages(UU("e")).size());
  

  ChatMessage message;
  message.date = 1583581800;
  message.user_id = UU("gsis");
  message.chat_room = UU("c");
  message.chat_message = UU("haha");
  EXPECT_EQ(true, chat_database_.StoreChatMessage(message));
  EXPECT_EQ(2, chat_database_.GetAllChatMessages(UU("c")).size());
}

TEST_F(ChatDatabaseTest, CreateChatRoom_AND_GetChatRoomList) {
  EXPECT_EQ(3, chat_database_.GetChatRoomList().size());
  chat_database_.CreateChatRoom(UU("d"));
  EXPECT_EQ(4, chat_database_.GetChatRoomList().size());
  chat_database_.CreateChatRoom(UU("d"));
  EXPECT_EQ(4, chat_database_.GetChatRoomList().size());
  chat_database_.CreateChatRoom(UU(""));
}

TEST(ChatDatabse, ParsingChatMessages) {
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

  file.open(chat_message_file, wofstream::out | ofstream::trunc);
  file << "1583581783kaist|aihi" << endl;
  file.close();
  EXPECT_EQ(false, chat_database.Initialize(chat_message_file, chat_room_file));

  file.open(chat_message_file, wofstream::out | ofstream::trunc);
  file << "1583581783|kaista|hihi" << endl;
  file.close();
  EXPECT_EQ(false, chat_database.Initialize(chat_message_file, chat_room_file));

  file.open(chat_message_file, wofstream::out | ofstream::trunc);
  file << "1583581783|kaist|ahihi" << endl;
  file.close();
  EXPECT_EQ(false, chat_database.Initialize(chat_message_file, chat_room_file));

  file.open(chat_message_file, wofstream::out | ofstream::trunc);
  file << "|||" << endl;
  file.close();
  EXPECT_EQ(false, chat_database.Initialize(chat_message_file, chat_room_file));

  file.open(chat_message_file, wofstream::out | ofstream::trunc);
  file << "1583581783|||" << endl;
  file.close();
  EXPECT_EQ(false, chat_database.Initialize(chat_message_file, chat_room_file));

  file.open(chat_message_file, wofstream::out | ofstream::trunc);
  file << "|kaist||" << endl;
  file.close();
  EXPECT_EQ(false, chat_database.Initialize(chat_message_file, chat_room_file));

  file.open(chat_message_file, wofstream::out | ofstream::trunc);
  file << "||a|" << endl;
  file.close();
  EXPECT_EQ(false, chat_database.Initialize(chat_message_file, chat_room_file));

  file.open(chat_message_file, wofstream::out | ofstream::trunc);
  file << "|||hello" << endl;
  file.close();
  EXPECT_EQ(false, chat_database.Initialize(chat_message_file, chat_room_file));

  file.open(chat_message_file, wofstream::out | ofstream::trunc);
  file << "1583581783|kaista|hihi|a|b" << endl;
  file.close();
  EXPECT_EQ(false, chat_database.Initialize(chat_message_file, chat_room_file));

  file.open(chat_message_file, wofstream::out | ofstream::trunc);
  file << "1583581783|kaista|hihi|a|b|c" << endl;
  file.close();
  EXPECT_EQ(false, chat_database.Initialize(chat_message_file, chat_room_file));
}

TEST(ChatDatabse, ParsingChatRooms) {
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

  file.open(chat_message_file, wofstream::out | ofstream::trunc);
  file << "1583581783|kaist|a|hihi" << endl;
  file.close();

  file.open(chat_room_file, wofstream::out | ofstream::trunc);
  file << "2307" << endl;
  file << "2308" << endl;
  file.close();
  EXPECT_EQ(true, chat_database.Initialize(chat_message_file, chat_room_file));

  file.open(chat_room_file, wofstream::out | ofstream::trunc);
  file << "2307" << endl;
  file << "2307" << endl;
  file.close();
  EXPECT_EQ(false, chat_database.Initialize(chat_message_file, chat_room_file));
}
