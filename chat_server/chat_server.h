// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#ifndef CHATSERVER_CHATSERVER_H_
#define CHATSERVER_CHATSERVER_H_

#include "cpprest/http_listener.h"
#include "cpprest/details/basic_types.h"

#include "account_database.h"
#include "session_manager.h"
#include "chat_database.h"

// This class is designed to run chat server with REST APIs.
// Please, call Initialize function before using this class.
// The chat server listens to four types of the HTTP request:
// GET, PUT, POST, DEL
// Example:
//   ChatServer chat_server(chat_database, acct_database, session_database);
//   chat_server.Initialize(server_address);
//   chat_server->OpenServer().wait();
//   ...
//   chat_server->CloseServer().wait();

namespace chatserver {
  
  class ChatServer {
   public:
    // Assign each parameter as a member variable.
    ChatServer(ChatDatabase* chat_database,
               AccountDatabase* account_database,
               SessionManager* session_manager);

    // Set-up http_listener that process incoming HTTP request using given URL.
    // Run session thread that removes an expired session.
    bool Initialize(utility::string_t server_url);

    // Open chat server. Return value, task<void>, means the chat server can be
    // executed asynchronously. Use wait() to run the task to open the chat
    // server.
    // Example:
    //   ChatServer chat_server;
    //   ...
    //   task_status status = chat_server.OpenServer().wait();
    pplx::task<void> OpenServer();

    // Close chat server. Return value, task<void>, means the chat server can be
    // closed asynchronously. Use wait() to run the task to close the chat
    // server.
    // Example:
    //   ChatServer chat_server;
    //   ...
    //   task_status status = chat_server.OpenServer().wait();
    //   ...
    //   status = chat_server.CloseServer().wait();
    pplx::task<void> CloseServer();

   private:
    // Processes ResetAPI GET requests that involve server inquiry. It handles
    // for getting chat messages and getting existing chat rooms.
    // RestAPI URL forms:
    // 1) get chat message list:
    //    http://server_url/chatmessage?chat_room=[]&session_id=[]
    // 2) get chat room list: http://server_url/chatroom?session_id=[]
    void HandleGet(const web::http::http_request& message);

    // Process incoming GET HTTP request for chat message list request.
    // <Parameter description>
    //  - message: Can make an HTTP reply to the incoming HTTP request.
    //  - url_queries: Hold query string of the incoming HTTP request URL.
    void ProcessGetChatMessageRequest(
        const web::http::http_request& message,
        const std::map<utility::string_t, utility::string_t>& url_queries);

    // Process incoming GET HTTP request for chat room list request.
    // <Parameter description>
    //  - message: Can make an HTTP reply to the incoming HTTP request.
    void ProcessGetChatRoomRequest(const web::http::http_request& message);

    // Processes ResetAPI POST requests that change server internal states.
    // It handles for account creations, login, accepting chat message, and
    // creating chat rooms.
    // RestAPI URL forms:
    // 1) make account: http://server_url/account?id=[]&pwd=[]
    // 2) login: http://server_url/login?id=[]&pwd=[]
    // 3) make chat message:
    //    http://server_url/chatmessage?chat_message=[]&chat_room=[]&session_id=[]
    // 4) make chat room:
    //    http://server_url/chatroom?chat_room=[]&session_id=[]
    void HandlePost(const web::http::http_request& message);

    // Process incoming POST HTTP request for sign-up.
    // <Parameter description>
    //  - message: Can make an HTTP reply to the incoming HTTP request.
    //  - url_queries: Hold query string of the incoming HTTP request URL.
    void ProcessPostSignUpRequest(
        const web::http::http_request& message, 
        const std::map<utility::string_t, utility::string_t>& url_queries);

    // Process incoming POST HTTP request for login.
    // <Parameter description>
    //  - message: Can make an HTTP reply to the incoming HTTP request.
    //  - url_queries: Hold query string of the incoming HTTP request URL.
    void ProcessPostLoginRequest(
        const web::http::http_request& message,
        const std::map<utility::string_t, utility::string_t>& url_queries);

    // Process incoming POST HTTP request for chat message input.
    // <Parameter description>
    //  - message: Can make an HTTP reply to the incoming HTTP request.
    //  - url_queries: Hold query string of the incoming HTTP request URL.
    void ProcessPostInputChatMessageRequest(
        const web::http::http_request& message, 
        const std::map<utility::string_t, utility::string_t>& url_queries);

    // Process incoming POST HTTP request for creating a chat room.
    // <Parameter description>
    //  - message: Can make an HTTP reply to the incoming HTTP request.
    //  - url_queries: Hold query string of the incoming HTTP request URL.
    void ProcessCreateChatRoomRequest(
        const web::http::http_request& message, 
        const std::map<utility::string_t, utility::string_t>& url_queries);

    // Listen to HTTP DELETE requests.
    // Mainly provides API to remove data in the web server.
    // API list:
    // 1) logout: http://server_url/session?session_id=[]
    void HandleDelete(const web::http::http_request& message);

    // Process incoming DELETE HTTP request for logout.
    // <Parameter description>
    //  - message: Can make an HTTP reply to the incoming HTTP request.
    //  - url_queries: Hold query string of the incoming HTTP request URL.
    void ProcessDeleteLogoutRequest(
        const web::http::http_request& message,
        const std::map<utility::string_t, utility::string_t>& url_queries);

    // Listen to HTTP PUT requests.
    // Mainly provides API to update data in the web server.
    // API list: none
    void HandlePut(const web::http::http_request& message);

    // Check the given session ID is valid or not. If the session is valid,
    // renew the alive time of the session.
    bool IsValidSession(
        const std::map<utility::string_t, utility::string_t>& url_queries);

    // HTTP_listener: can listen to HTTP requests.
    web::http::experimental::listener::http_listener listener_;

    // chat_database_ manages every chat message and room.
    ChatDatabase* chat_database_;

    // account_database_ manages every user account information.
    AccountDatabase* account_database_;

    // session_manager_ manages every session information for a user account.
    SessionManager* session_manager_;
  };

} // namespace chatserver

#endif CHATSERVER_CHATSERVER_H_ // CHATSERVER_CHATSERVER_H_