// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#include "chat_room.h"

#include <future>

#include "spdlog/spdlog.h"

using namespace std;
using ::chatserver::ChatMessage;
using ::web::http::methods;
using ::web::http::http_response;
using ::web::http::status_codes;
using ::web::json::value;
using ::web::json::array;
using ::utility::string_t;
using ::utility::ostringstream_t;
using ::spdlog::error;
using chrono::duration;

namespace chatclient {

  // Maximum number of chat messages to be displayed on the screen.
  const time_t kMaxDisplayChatMessages = 5;
  // Line number to receive chat message input.
  const time_t kInputLine = 10;
  // Polling interval to make chat message request to the server.
  const duration<int> kPollingInterval = std::chrono::seconds(1);

  ChatRoom::ChatRoom(string_t chat_server_url,
                     string_t session_id, 
                     string_t current_chat_room)
                     : session_id_(session_id),
                       current_chat_room_(current_chat_room) {
    http_requester_ = make_unique<HttpRequester>(
        web::http::uri_builder(chat_server_url).to_uri().to_string());
    chat_room_view_ = make_unique<ChatRoomView>();
  }

  ChatRoom::~ChatRoom() {
    run_display_thread_ = false;
  }

  void ChatRoom::RunChatRoom() {
    chat_room_view_->ClearConsole();
    chat_room_view_->SetCursorPosition(0, kInputLine);
    run_display_thread_ = true;
    // Run polling thread to receive chat messages from the chat server.
    // Getting return value is necessary to run async thread
    // but the value is not used.
    async_thread_result_ = async(launch::async, 
                                 &ChatRoom::PollingChatMessageFromServer, this);
    // Run receiving chat messages from the user through C++ Standard input.
    ProcessChatMessageInput();
  }

  void ChatRoom::PollingChatMessageFromServer() {
    vector<ChatMessage> chat_messages;
    while (run_display_thread_) {
      chat_messages.clear();
      // Make an HTTP request to get chat messages from the server.
      if (GetChatMessagesFromServer(chat_messages)) {
        // Receive the number of new messages to be displayed.
        const size_t new_chat_message_size = 
            ComputeNewChatMessageSize(chat_messages);

        // Delete previous messages that are not fit the window size of the
        // chat message display.
        RemoveChatMessages(new_chat_message_size);

        // Add new chat messages to be displayed.
        AddNewChatMessages(new_chat_message_size, chat_messages);

        // Display chat messages when there are new chat messages.
        if (new_chat_message_size > 0) {
          chat_room_view_->
              DisplayChatMessages(display_chat_message_, 
                                  current_chat_room_, 
                                  kMaxDisplayChatMessages);
        }
      } else {
        error("Fail to get chat messages.");
      }
      this_thread::sleep_for(kPollingInterval);
    }
  }

  void ChatRoom::ProcessChatMessageInput() {
    while (true) {
      chat_room_view_->SetCursorPosition(0, kInputLine);
      ostringstream_t buf;
      buf << "You are in [" << current_chat_room_ << "]. "
          << "Enter quit to leave the chat room.";
      chat_room_view_->DisplayMessage(buf.str());
      string_t chat_message = 
          chat_room_view_->GetUserInput(UU("Input chat message: "));
      // Clear entered chat messages on the console screen.
      chat_room_view_->ClearConsole(kInputLine, kInputLine + 2);
      if (chat_message == UU("quit")) {
        chat_room_view_->DisplayMessage(UU("Quit chat room"));
        chat_room_view_->ClearConsole();
        chat_room_view_->SetCursorPosition(0, 0);
        run_display_thread_ = false;
        break;
      } 

      if (StoreChatMessagesToServer(chat_message)) {
        chat_room_view_->
            DisplayChatMessages(display_chat_message_, 
                                current_chat_room_, 
                                kMaxDisplayChatMessages);
      } 
    }
  }
  
  size_t ChatRoom::ComputeNewChatMessageSize(
    const vector<ChatMessage>& chat_messages) {
    if (display_chat_message_.size() == 0) {
      if (chat_messages.size() >= kMaxDisplayChatMessages) {
        return kMaxDisplayChatMessages;
      } else {
        return chat_messages.size();
      }
    }

    // Calculate new chat messages from the server.
    size_t new_chat_message_size = 0;
    const ChatMessage last_displayed_chat_message =
        display_chat_message_.back();
    for (size_t i = chat_messages.size() - 1; i >= 0; i--) {
      const ChatMessage chat_message = chat_messages.at(i);
      if (chat_message != last_displayed_chat_message) {
        new_chat_message_size++;
        if (new_chat_message_size >= kMaxDisplayChatMessages) {
          break;
        }
      } else {
        break;
      }
    }
    return new_chat_message_size;
  }

  bool ChatRoom::GetChatMessagesFromServer(
      vector<ChatMessage>& chat_messages) const {
    // Make request URL and body data.
    ostringstream_t http_request_url;
    http_request_url.clear();
    http_request_url << "chatmessage" << UU("?session_id=") << session_id_
                     << UU("&chat_room=") << current_chat_room_;

    // Make HTTP request to get chat messages.
    const http_response response =
        http_requester_->MakeHttpRequestForResponse(methods::GET,
                                                    http_request_url.str());
    if (response.status_code() == status_codes::OK) {
      ::array chat_list = response.extract_json().get().as_array();
      for (auto& i : chat_list) {
        chat_messages.emplace_back(
            i[UU("date")].as_number().to_uint64(),
            i[UU("user_id")].as_string(),
            i[UU("room")].as_string(),
            i[UU("message")].as_string());
      }
      return true;
    }
    return false;
  }

  bool ChatRoom::StoreChatMessagesToServer(const string_t chat_message) const {
    // Make request URL and body data.
    ostringstream_t http_request_url;
    http_request_url << "chatmessage";
    value body_data;
    body_data[UU("chat_message")] = value::string(chat_message);
    body_data[UU("chat_room")] = value::string(current_chat_room_);
    body_data[UU("session_id")] = value::string(session_id_);

    // When the HTTP request succeeds, return true otherwise return false.
    const http_response response = http_requester_->MakeHttpRequestForResponse(
        methods::POST, http_request_url.str(), body_data);
    if (response.status_code() == status_codes::OK) {
      return true;
    }
    return false;
  }

  void ChatRoom::RemoveChatMessages(size_t new_chat_message_size) {
    const size_t remove_chat_message_size = display_chat_message_.size() + 
                                            new_chat_message_size - 
                                            kMaxDisplayChatMessages;
    for (size_t i = 0; i < remove_chat_message_size; i++) {
      if (display_chat_message_.size() > 0) {
        display_chat_message_.pop_front();
      } else {
        break;
      }
    }
  }

  void ChatRoom::AddNewChatMessages(size_t new_chat_message_size, 
                                    const vector<ChatMessage>& chat_messages) {
    for (size_t i = 0; i < new_chat_message_size; i++) {
      display_chat_message_.push_back(chat_messages.at(
          i + chat_messages.size() - new_chat_message_size));
    }
  }

} // namespace chatclient