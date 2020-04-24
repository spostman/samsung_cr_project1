#ifndef CHATSERVER_CHATMESSAGE_H_
#define CHATSERVER_CHATMESSAGE_H_

#include <ctime>

#include "cpprest/details/basic_types.h"

namespace chatserver {
  // Chat message information structure (date, user_id, chat_room, chat_message)
  struct ChatMessage {
    // Chat message input time
    time_t date;
    // Who is generating chat message
    utility::string_t user_id;
    // What chat room was created in the chat room
    utility::string_t chat_room;
    // Chat message contents
    utility::string_t chat_message;
  };
} // namespace chatserver
#endif CHATSERVER_CHATMESSAGE_H_ // CHATSERVER_CHATMESSAGE_H_