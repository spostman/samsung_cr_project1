// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#ifndef CHATCLIENT_CHATCLIENT_H_
#define CHATCLIENT_CHATCLIENT_H_

#include "chat_client_view.h"
#include "chat_room.h"
#include "http_requester.h"
#include "cpprest/http_client.h"

// This class receives commands from the user through the C++ standard input.
// Each command initiates communicating with the chat server via a HTTP request
// and displays its server response to the user through the C++ standard
// output.
// Main commands are:
//  - login, signup, logout, chat room create, and chat message input
//
// Usage Example:
//   ChatClient chat_client(chat_server_url);
//   chat_client.run()

namespace chatclient {

  class ChatClient {
    friend class ChatClientTest;
   public:
    // Initialize an chat client that sends RestAPI HTTP HTTP requests to
    // a chat server using chat_server_url.
    ChatClient(utility::string_t chat_server_url);

    // Starts a chat client. This function acts as a router that calls the
	  // appropriate function based on the command received from the user.
	  // It uses stdinput/stdoutput to process user input. The user interface
	  // is governed by the internal client status (current_client_status_).
    void RunChatClient();

    // The ClientStatus means what stage the user is in the chat client.
    // The current_client_status_ variable holds the ClientStatus.
    // There are three chat client states.
    //  1) kBeforeLogin : process login, signup.
    //  2) kAfterLogin: process chat room join, chat room create, logout.
    //  3) kInChatRoom: process chat message exchange to a server.
    typedef enum {
      kBeforeLogin,
      kAfterLogin,
      kInChatRoom
    } ClientStatus;

   private:
     bool BeforeLogin();
     bool AfterLogin();
     void InChatRoom();

    // Make an HTTP request to the chat server for signup.
    // Process user input for ID and password.
    void SignUp() const;

    // Make an HTTP request to the chat server for login.
    // Process user input for ID and password.
    // Change client status to afterLogin, when login is successful.
    void Login();

    // Make an HTTP request to the chat server to get a list of chat rooms.
    // Display the list of chat rooms.
    web::http::status_code DisplayChatRoomList() const;

    // Make an HTTP request to the chat server to create a chat room
    // Process user input for a chat room to create
    web::http::status_code CreateChatRoom() const;

    // Enter a chat room. Change current_chat_room to given chat room.
    // Process user input for a chat room to join.
    // Change client status to kInChatRoom.
    void JoinChatRoom();

    // Make an HTTP request to the chat server to logout.
    // Change client status to beforeLogin.
    // Reset user information: session_id, user_id, current_chat_room.
    void Logout();

    // Check whether the given chat room exists in the chat server.
    bool IsExistingChatRoom(utility::string_t chat_room) const;

    // Clear user information.
    void ClearUserInformation();

    // Chat message input, chat message display, and
    // http request/response process
    // in the chat room are all processed through this variable.
    std::unique_ptr<ChatRoom> chat_room_;

    // HttpRequester: can make an HTTP request to the chat server.
    // All requests to the chat server are made through this variable.
    std::unique_ptr<HttpRequester> http_requester_;

    // Display message and process user input of chat client.
    std::unique_ptr<ChatClientView> chat_client_view_;

    // User session ID
    utility::string_t session_id_;

    // User ID
    utility::string_t user_id_;

    // Current chat room
    utility::string_t current_chat_room_;

    // Current client status
    ClientStatus current_client_status_;

    // Chat server URL
    utility::string_t chat_server_url_;
  };

} // namespace chatclient

#endif // CHATCLIENT_CHATCLIENT_H_
