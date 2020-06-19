// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#ifndef CHATCLIENT_CHATROOM_H_
#define CHATCLIENT_CHATROOM_H_

#include <future>

#include "cpprest/http_client.h"
#include "chat_message.h"
#include "chat_room_view.h"
#include "http_requester.h"

// This class is designed to process all functions, when the user is in a chat
// room. There are two main functions:
//   1) Receive chat messages from the chat server using polling and display
//      chat messages on the console screen. 
//   2) Receives chat messages from the user through C++ Standard input,
//      delivers it to the chat server, and displays it on the screen.
// It uses "asynchronous threads" to run polling thread to get a chat message.
// It removes the message on the console screen and manipulates the location
// of the console input cursor.
// Example:
//   ChatRoom chat_room(chat_server_url_, session_id_, current_chat_room_);
//   chat_room.run()

namespace chatclient {

  class ChatRoom {
   public:
    // Initialize the http_requester that can make HTTP requests to the chat
    // server using chat_server_url.
    ChatRoom(utility::string_t chat_server_url, 
             utility::string_t session_id, 
             utility::string_t current_chat_room);

    // Stop polling thread.
    ~ChatRoom();

    // Please call this function to start a chat room. It runs two functions:
    //   1) Polling chat messages from the server (PollingChatMessageFromServer).
    //   2) Process user input to make chat messages (ProcessChatMessageInput).
    void RunChatRoom();

   private:
    // Make an HTTP request to the chat server every kPollingInterval. If there
    // are new messages, displayed the messages to the console screen.
    void PollingChatMessageFromServer();

    // Receive the user's chat message input. Make an HTTP request to save the
    // chat messages to the chat server. Display the entered chat message on
    // the console screen.
    void ProcessChatMessageInput();
    
    // It returns the number of new chat messages to be displayed to the console
    // screen. Calculate the number of chat messages to be newly displayed by
    // comparing the chat messages received from the server with the currently
    // displayed chat message.
    size_t ComputeNewChatMessageSize(
        const std::vector<chatserver::ChatMessage>& chat_messages);

    // Get chat messages from the server using HTTP request. Store the chat
    // messages to the given vector.
    bool GetChatMessagesFromServer(
        std::vector<chatserver::ChatMessage>& chat_messages) const;

    // Store the chat message to the chat server. The chat message sends to the
    // chat server via an HTTP request.
    bool StoreChatMessagesToServer(utility::string_t chat_message) const;

    // Remove old chat messages in display_chat_message_ by the given number.
    void RemoveChatMessages(size_t new_chat_message_size);

    // Stores the latest chat messages in display_chat_message_ as many as the
    // given number from given chat_messages.
    void AddNewChatMessages(
        size_t new_chat_message_size, 
        const std::vector<chatserver::ChatMessage>& chat_messages);
    
    // HttpRequester: can make an HTTP request to the chat server. All requests
    // to the chat server are made through this variable.
    std::unique_ptr<HttpRequester> http_requester_;

    // Display message and process user input of chat room.
    std::unique_ptr<ChatRoomView> chat_room_view_;

    // User session ID.
    utility::string_t session_id_;

    // User ID.
    utility::string_t user_id_;

    // Current chat room.
    utility::string_t current_chat_room_;

    // Chat messages currently displayed on the console screen.
    std::list<chatserver::ChatMessage> display_chat_message_;

    // It controls the async thread to be stopped and start.
    bool run_display_thread_;

    // It is used to maintain the asynchronous state of the async thread.
    std::future<void> async_thread_result_;
  };

} // namespace chatclient

#endif // CHATCLIENT_CHATROOM_H_