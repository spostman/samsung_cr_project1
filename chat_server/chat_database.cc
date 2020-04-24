#include "chat_database.h"

#include "cpprest/asyncrt_utils.h"
#include "spdlog/spdlog.h"

namespace chatserver {

  using namespace std;
  using ::utility::conversions::to_utf8string;
  using ::utility::string_t;
  using ::spdlog::error;

  bool ChatDatabase::Initialize(string_t chat_message_file,
                                string_t chat_room_file) {
    chat_message_file_ = chat_message_file;
    chat_room_file_ = chat_room_file;

    if (!ReadChatMessagesFromFileDatabase(chat_message_file_)) {
      error("Error to open chat message file: {}",
            to_utf8string(chat_message_file_));
      return false;
    }

    if (!ReadChatRoomFromFileDatabase(chat_room_file_)) {
      error("Error to open chat room file: {}", 
            to_utf8string(chat_room_file_));
      return false;
    }
    return true;
  }

  bool ChatDatabase::StoreChatMessage(ChatMessage message) {
    wofstream file(chat_message_file_,
                   wofstream::out | wofstream::app);
    if (file.is_open()) {
      // File format: date|user_id|chat_room|chat_message
      file << message.date << kParsingDelimeter
           << message.user_id << kParsingDelimeter
           << message.chat_room << kParsingDelimeter
           << message.chat_message << endl;
      file.close();
      chat_messages_[message.chat_room].push_back(message);
    } else {
      error("Unable to open file: {}", to_utf8string(chat_message_file_));
      return false;
    }
    return true;
  }

  const vector<ChatMessage>& ChatDatabase::GetAllChatMessages(
      string_t chat_room) {
    if(chat_messages_.find(chat_room) != chat_messages_.end()) {
      return chat_messages_[chat_room];
    } else {
      static vector<ChatMessage> chat_messages;
      return chat_messages;
    }
  }

  bool ChatDatabase::CreateChatRoom(string_t chat_room) {
    if (chat_room.size() == 0) {
      error("Chat room name cannot be zero length");
    } else if (IsExistChatRoom(chat_room)) {
      error("Chat room name already exists");
      return false;
    }

    wofstream file(chat_room_file_, wofstream::out | wofstream::app);
    if (file.is_open()) {
      file << chat_room << endl;
      file.close();
      chat_rooms_.push_back(chat_room);
      return true;
    } else {
      error("Unable to open file: {}", to_utf8string(chat_room_file_));
      return false;
    }
  }

  bool ChatDatabase::IsExistChatRoom(string_t chat_room) const {
    if (find(chat_rooms_.begin(), chat_rooms_.end(), chat_room) == 
        chat_rooms_.end()) {
      return false;
    } else {
      return true;
    }
  }

  const vector<string_t>& ChatDatabase::GetChatRoomList() const{
    return chat_rooms_;
  }

  bool ChatDatabase::ReadChatMessagesFromFileDatabase(
      string_t chat_message_file) {
    wifstream file(chat_message_file);
    if (!file.is_open()) {
      error("Can't open chat message file: {}",
            to_utf8string(chat_message_file));
      return false;
    }

    if (!ParsingChatMessageFile(move(file))) {
      error("Parsing error: {}", to_utf8string(chat_message_file));
      file.close();
      return false;
    }
    file.close();
    return true;
  }

  bool ChatDatabase::ParsingChatMessageFile(wifstream chat_message_file) {
    string_t line;
    while (chat_message_file.good()) {
      getline(chat_message_file, line);
      if (line.length() == 0) continue;

      size_t end_index = 0;
      size_t start_index = 0;
      ChatMessage message;

      // Parsing format: date|user_id|chat_room|chat_message
      for (auto i = 0; i < 3; i++) {
        end_index = line.find(kParsingDelimeter, start_index);
        if (end_index == start_index) {
          error("Chat message file parsing error");
          return false;
        }

        if (i == 0) {
          message.date = stoll(
              line.substr(start_index, end_index - start_index));
        } else if (i == 1) {
          message.user_id = line.substr(start_index, end_index - start_index);
        } else if (i == 2) {
          message.chat_room = line.substr(start_index, end_index - start_index);
        }
        start_index = end_index + 1;

        if (start_index == line.length() || 
            end_index == string_t::npos) {
          error("Chat message file parsing error");
          return false;
        }
      }

      message.chat_message = line.substr(start_index);
      // Too many delimiters in the line.
      if (message.chat_message.find(kParsingDelimeter) != string_t::npos) {
        error("Chat message file parsing error");
        return false;
      }
      chat_messages_[message.chat_room].push_back(message);
    }
    return true;
  }

  bool ChatDatabase::ReadChatRoomFromFileDatabase(string_t chat_room_file) {
    string_t line;
    wifstream file(chat_room_file);
    if (!file.is_open()) {
      error("Can't open chat room file: {}", to_utf8string(chat_room_file));
      return false;
    }

    while (file.good()) {
      getline(file, line);
      if (line.length() == 0) continue;
      if (!IsExistChatRoom(line)) {
        chat_rooms_.push_back(line);
      } else {
        error("Duplicate chat room name");
        return false;
      }
    }
    file.close();
    return true;
  }
} // namespace chatserver
