// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#ifndef CHATSERVER_CHATDATABASE_H_
#define CHATSERVER_CHATDATABASE_H_

#include <map>
#include <vector>

#include "cpprest/details/basic_types.h"
#include "chat_message.h"

namespace chatserver {

  // This class is designed to manage chat messages and rooms.
  // It uses two file databases for chat messages and rooms.
  // Example:
  //   ChatDatabase chat_database;
  //   account_database.Initialize("chat_message_db.txt", "chat_room_db.txt");
  //   ChatMessage message;
  //   message.user = "kaist"; message.date = now(); message.message = "hihi";
  //   message.chat_room = "gsis";
  //
  //   if(chat_database.IsExistChatRoom(message.chat_room)) {
  //     if(chat_database.StoreChatMessage(message)) {
  //       do something to success save the chat message
  //     } else {
  //       do something to fail to make the chat message
  //     }
  //   } else {
  //     do something to fail to find the given chat room
  //   }
  //
  //   if (chat_database.CreateChatRoom(message.chat_room)) {
  //     do something to success to make the chat room
  //   } else {
  //     do something to fail to create the chat room
  //   }
  // The usage with GetChatList() function is similar to the above.
  
  class ChatDatabase {
  public:
    // Read chat messages and chat rooms from given file into database.
    bool Initialize(utility::string_t chat_message_file,
                    utility::string_t chat_room_file);
    // Store chat message on the database.
    bool StoreChatMessage(ChatMessage message);
    // Get all chat messages in the given chat room trough out_chat_messages.
    const std::vector<ChatMessage>& GetAllChatMessages(
        utility::string_t chat_room);
    // Create the chat room.
    bool CreateChatRoom(utility::string_t chat_room);
    // Check the given chat room exists.
    bool IsExistChatRoom(utility::string_t chat_room) const;
    // Get every chat room list.
    const std::vector<utility::string_t>& GetChatRoomList() const;
    // Delimiter in the chat message file database.
    const utility::string_t kParsingDelimeter = UU("|");

  private:
    // Read chat messages from the given file into database.
    bool ReadChatMessagesFromFileDatabase(utility::string_t chat_message_file);
    // Parse chat message file. Format: date|user_id|chat_room|chat_message.
    bool ParsingChatMessageFile(std::wifstream chat_message_file);
    // Read chat rooms from the given file into database.
    bool ReadChatRoomFromFileDatabase(utility::string_t chat_room_file);

    // Chat message database: std::map<chat room, ChatMessage>.
    std::map<utility::string_t, std::vector<ChatMessage>> chat_messages_;
    // Chat room.
    std::vector<utility::string_t> chat_rooms_;
    // Chat message file database name.
    utility::string_t chat_message_file_;
    // Chat room file database name.
    utility::string_t chat_room_file_;
  };
} // namespace chatserver
#endif CHATSERVER_CHATDATABASE_H_ // CHATSERVER_CHATDATABASE_H_
