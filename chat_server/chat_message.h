// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#ifndef CHATSERVER_CHATMESSAGE_H_
#define CHATSERVER_CHATMESSAGE_H_

#include <ctime>

#include "cpprest/details/basic_types.h"

// Chat message information structure (date, user_id, chat_room, chat_message)

namespace chatserver {

  struct ChatMessage {
    // Constructor with no parameters.
    ChatMessage() {
    }

    // Constructor with all parameters.
    ChatMessage(std::time_t date, 
                utility::string_t user_id, 
                utility::string_t chat_room, 
                utility::string_t chat_message)
                : date(date),
                  user_id(user_id),
                  chat_room(chat_room),
                  chat_message(chat_message) {
    }

    // Chat message input time
    std::time_t date;

    // Who is generating chat message
    utility::string_t user_id;

    // What chat room was created in the chat room
    utility::string_t chat_room;

    // Chat message contents
    utility::string_t chat_message;

    bool operator==(const ChatMessage& compare_chat_message) const {
      return (compare_chat_message.chat_room == chat_room &&
              compare_chat_message.chat_message == chat_message &&
              compare_chat_message.user_id == user_id &&
              compare_chat_message.chat_room == chat_room);
    }

    bool operator!=(const ChatMessage& compare_chat_message) const {
      return (compare_chat_message.chat_room != chat_room ||
              compare_chat_message.chat_message != chat_message ||
              compare_chat_message.user_id != user_id ||
              compare_chat_message.chat_room != chat_room);
    }
  };

} // namespace chatserver

#endif CHATSERVER_CHATMESSAGE_H_ // CHATSERVER_CHATMESSAGE_H_