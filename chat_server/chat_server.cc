// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#include "chat_server.h"

#include "cpprest/json.h"
#include "cpprest/uri.h"
#include "spdlog/spdlog.h"

using namespace std;
using ::web::uri;
using ::web::http::methods;
using ::web::http::http_request;
using ::web::http::status_codes;
using ::web::http::experimental::listener::http_listener;
using ::web::json::value;
using ::utility::string_t;
using ::utility::conversions::to_utf8string;
using ::pplx::task;
using ::spdlog::info;
using ::spdlog::warn;
using ::spdlog::error;

namespace chatserver {

  ChatServer::ChatServer(ChatDatabase* chat_database, 
                         AccountDatabase* account_database, 
                         SessionManager* session_manager)
                         : chat_database_(chat_database),
                           account_database_(account_database),
                           session_manager_(session_manager) {
  }

  bool ChatServer::Initialize(string_t server_url) {
    if (chat_database_ == nullptr || 
        account_database_ == nullptr || 
        session_manager_ == nullptr) {
      error("chat_database, account_database, or session_manager is nullptr.");
      return false;
    }
    session_manager_->RunSessionExpireThread();

    // HTTP request listener from cpprestsdk.
    listener_ = http_listener(server_url);  

    // Set HTTP request methods.
    listener_.support(methods::GET,
                      bind(&ChatServer::HandleGet, this, placeholders::_1));
    listener_.support(methods::PUT,
                      bind(&ChatServer::HandlePut, this, placeholders::_1));
    listener_.support(methods::POST,
                      bind(&ChatServer::HandlePost, this, placeholders::_1));
    listener_.support(methods::DEL,
                      bind(&ChatServer::HandleDelete, this, placeholders::_1));
    return true;
  }

  task<void> ChatServer::OpenServer() {
    return listener_.open();
  }

  task<void> ChatServer::CloseServer() {
    return listener_.close();
  }

  void ChatServer::HandleGet(const http_request& message) {
    // Path of HTTP request URL.
    // path[n] means the name of the nth path in HTTP request URL.
    vector<string_t> url_paths = uri::split_path(
        uri::decode(message.relative_uri().path()));
    
    // Query string of HTTP request URL.
    // Format: map<string_t, string_t> = <query name, query value>
    map<string_t, string_t> url_queries = uri::split_query(
        uri::decode(message.relative_uri().query()));

    if (url_paths.empty()) {
      message.reply(status_codes::NotFound);
      return;
    }

    if (!IsValidSession(url_queries)) {
      message.reply(status_codes::Forbidden,
                    UU("Not a valid session ID"));
      return;
    }

    // Process API service: get chat message, get chat room list.
    string_t first_request_url_path = url_paths[0];
    if (first_request_url_path == UU("chatmessage")) {
      ProcessGetChatMessageRequest(message, url_queries);
      return;
    } else if (first_request_url_path == UU("chatroom")) {
      ProcessGetChatRoomRequest(message);
      return;
    }

    // No matching HTTP request.
    warn("No matching HTTP request");
    message.reply(status_codes::NotFound);
  }

  void ChatServer::ProcessGetChatMessageRequest(
      const http_request& message,
      const map<string_t, string_t>& url_queries) {
    const auto chat_room_it = url_queries.find(UU("chat_room"));
    if (chat_room_it == url_queries.end()) {
      message.reply(status_codes::BadRequest,
                    UU("Chat room information missing"));
      return;
    }

    if (!chat_database_->IsExistChatRoom(chat_room_it->second)) {
      message.reply(status_codes::BadRequest,
                    UU("There are no chat rooms: ") + chat_room_it->second);
      return;
    }

    const vector<ChatMessage>* chat_messages =
        chat_database_->GetAllChatMessages(chat_room_it->second);
    value result = value::array(); // Body data for HTTP response.
    size_t idx = 0;
    for (auto chat_message = chat_messages->begin(); 
         chat_message != chat_messages->end(); 
         ++chat_message) {
      value json_obj = value::object();
      json_obj[UU("date")] = value::number(chat_message->date);
      json_obj[UU("user_id")] = value::string(chat_message->user_id);
      json_obj[UU("message")] = value::string(
          chat_message->chat_message);
      json_obj[UU("room")] = value::string(chat_message->chat_room);

      result[idx++] = json_obj;
    }

    message.reply(status_codes::OK, result);
    return;
  }

  void ChatServer::ProcessGetChatRoomRequest(const http_request& message) {
    const vector<string_t>* chat_room_list = chat_database_->GetChatRoomList();
    value result = value::array();  // Body data for HTTP response.
    size_t idx = 0;
    for (auto room = chat_room_list->begin(); 
         room != chat_room_list->end();
         ++room) {
      value json_obj = value::object();

      json_obj[UU("room")] = value::string(room->data());
      result[idx++] = json_obj;
    }
    message.reply(status_codes::OK, result);
  }

  void ChatServer::HandlePost(const http_request& message) {
    // Path of HTTP request URL.
    // path[n] means the name of the nth path in HTTP request URL.
    vector<string_t> url_paths = uri::split_path(
        uri::decode(message.relative_uri().path()));

    // Query string of HTTP request URL.
    // Format: map<string_t, string_t> = <query name, query value>
    map<string_t, string_t> url_queries = uri::split_query(
        uri::decode(message.relative_uri().query()));

    if (url_paths.empty()) {
      message.reply(status_codes::NotFound);
      return;
    }

    // API service without session ID.
    const string_t first_request_url_path = url_paths[0];
    if (first_request_url_path == UU("account")) {
      ProcessPostSignUpRequest(message, url_queries);
      return;
    } else if (first_request_url_path == UU("login")) {
      ProcessPostLoginRequest(message, url_queries);
      return;
    }

    if (!IsValidSession(url_queries)) {
      message.reply(status_codes::Forbidden,
                    UU("Not a valid session ID"));
      return;
    }

    // Function call according to URL path with session ID.
    const string_t second_request_url_path = url_paths[0];
    if (second_request_url_path == UU("chatmessage")) {
      ProcessPostInputChatMessageRequest(message, url_queries);
      return;
    } else if (second_request_url_path == UU("chatroom")) {
      ProcessCreateChatRoomRequest(message, url_queries);
      return;
    }

    // No matching HTTP request.
    warn("No matching HTTP request");
    message.reply(status_codes::NotFound);
  }

  void ChatServer::ProcessPostSignUpRequest(
      const http_request& message,
      const map<string_t, string_t>& url_queries) {
    const auto id_it = url_queries.find(UU("id"));
    const auto password_it = url_queries.find(UU("password"));
    if (id_it == url_queries.end() ||
      password_it == url_queries.end()) {
      message.reply(status_codes::BadRequest,
          UU("Account information absence"));
      return;
    }
    const string_t id = id_it->second;
    const string_t password = password_it->second;

    const int signup_result = 
        account_database_->SignUp(id, password);
    if (signup_result == AccountDatabase::kAuthSuccess) {
      message.reply(status_codes::OK);
      return;
    } else if (signup_result == AccountDatabase::kProhibitedCharInID) {
      message.reply(status_codes::BadRequest,
                    UU("Prohibited character in ID"));
      return;
    } else if (signup_result == AccountDatabase::kProhibitedCharInPassword) {
      message.reply(status_codes::BadRequest,
                    UU("Prohibited character in password"));
      return;
    } else if (signup_result == AccountDatabase::kDuplicateID) {
      message.reply(status_codes::BadRequest, 
                    UU("Duplicated ID"));
      return;
    } else if (signup_result == AccountDatabase::kAccountWriteError) {
      message.reply(status_codes::BadRequest,
                    UU("Account write error in file DB"));
      return;
    }
  }

  void ChatServer::ProcessPostLoginRequest(
      const http_request& message, 
      const map<string_t, string_t>& url_queries) {
    const auto id_it = url_queries.find(UU("id"));
    const auto password_it = url_queries.find(UU("password"));
    const auto nonce_it = url_queries.find(UU("nonce"));
    if (id_it == url_queries.end() || 
        password_it == url_queries.end() ||
        nonce_it == url_queries.end()) {
      message.reply(status_codes::BadRequest,
                    UU("Account information absence"));
      return;
    }
    
    const string_t id = id_it->second;
    const string_t password = password_it->second;
    const string_t nonce = nonce_it->second;

    const int login_result = 
        account_database_->Login(id, password, nonce);
    if (login_result == AccountDatabase::kAuthSuccess) {
      const Session session = session_manager_->CreateSession(id);
      value result = value::object();  // Body data for HTTP response.
      result[UU("session_id")] = value::string(session.session_id);
      message.reply(status_codes::OK, result);
      return;
    } else if (login_result == AccountDatabase::kIDNotExist) {
      message.reply(status_codes::BadRequest, UU("ID not exist"));
      return;
    } else if (login_result == AccountDatabase::kPasswordError) {
      message.reply(status_codes::BadRequest,
                    UU("Password error"));
      return;
    }
  }

  void ChatServer::ProcessPostInputChatMessageRequest(
      const http_request& message, 
      const map<string_t, string_t>& url_queries) {
    if (url_queries.find(UU("chat_message")) == url_queries.end() ||
      url_queries.find(UU("chat_room")) == url_queries.end()) {
      message.reply(status_codes::BadRequest,
                    UU("Chat information absence"));
      return;
    }

    const string_t session_id = url_queries.find(UU("session_id"))->second;
    ChatMessage chat_message;
    if (!session_manager_->GetUserIDFromSessionId(session_id, 
                                                  &chat_message.user_id)) {
      message.reply(status_codes::InternalError,
                    UU("Can't find user ID from given session ID"));
      return;
    }

    chat_message.chat_message = url_queries.find(UU("chat_message"))->second;
    chat_message.chat_room = url_queries.find(UU("chat_room"))->second;
    chat_message.date = chrono::system_clock::to_time_t(
        chrono::system_clock::now());
    if (chat_database_->StoreChatMessage(chat_message)) {
      message.reply(status_codes::OK);
      return;
    } else {
      message.reply(status_codes::InternalError, 
                    UU("Fail to store chat message"));
      return;
    }
  }

  void ChatServer::ProcessCreateChatRoomRequest(
      const http_request& message,
      const map<string_t, string_t>& url_queries) {
    if (url_queries.find(UU("chat_room")) == url_queries.end()) {
      message.reply(status_codes::BadRequest, UU("No chat room parameter"));
      return;
    }

    if (chat_database_->CreateChatRoom(
        url_queries.find(UU("chat_room"))->second)) {
      message.reply(status_codes::OK);
    } else {
      message.reply(status_codes::BadRequest);
    }
  }

  void ChatServer::HandleDelete(const http_request& message) {
    // Path of HTTP request URL.
    // path[n] means the name of the nth path in HTTP request URL.
    vector<string_t> url_paths = uri::split_path(
        uri::decode(message.relative_uri().path()));

    // Query string of HTTP request URL.
    // Format: map<string_t, string_t> = <query name, query value>
    map<string_t, string_t> url_queries = uri::split_query(
        uri::decode(message.relative_uri().query()));

    if (!IsValidSession(url_queries)) {
      message.reply(status_codes::Forbidden,
                    UU("Not a valid session ID"));
      return;
    }

    // Function call according to URL path.
    if (url_paths[0] == UU("session")) {
      ProcessDeleteLogoutRequest(message, url_queries);
      return;
    }

    // No matching HTTP request.
    warn("No matching HTTP request");
    message.reply(status_codes::NotFound);
  }

  void ChatServer::ProcessDeleteLogoutRequest(
      const http_request& message, 
      const map<string_t, string_t>& url_queries) {

    const auto session_id = url_queries.find(UU("session_id"))->second;
    const auto result = session_manager_->DeleteSession(session_id);
    if (result) {
      message.reply(status_codes::OK);
      return;
    } else {
      message.reply(status_codes::NotFound);
      return;
    }
  }

  void ChatServer::HandlePut(const http_request& message) {
    // No matching HTTP request.
    warn("No matching HTTP request");
    message.reply(status_codes::NotFound);
  }

  bool ChatServer::IsValidSession(
      const map<string_t, string_t>& url_queries) {
    const auto session_id_it = url_queries.find(UU("session_id"));
    if (session_id_it == url_queries.end() ||
      !session_manager_->IsExistSessionId(session_id_it->second)) {
      return false;
    } else {
      // If there is a request through a valid session id,
      // renew the session alive time.
      if (!session_manager_->RenewLastActivityTime(session_id_it->second)) {
        return false;
      }
    }
    return true;
  }

} // namespace chatserver
