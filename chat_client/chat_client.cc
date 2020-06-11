// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#include "chat_client.h"

#include "spdlog/spdlog.h"

using namespace std;
using ::chatserver::ChatMessage;
using ::web::uri;
using ::web::http::methods;
using ::web::http::http_response;
using ::web::http::status_code;
using ::web::http::status_codes;
using ::web::json::value;
using ::web::json::array;
using ::utility::string_t;
using ::utility::ostringstream_t;
using ::utility::conversions::to_string_t;
using ::spdlog::error;

namespace chatclient {

  ChatClient::ChatClient(const string_t chat_server_url)
      : current_client_status_(kBeforeLogin),
        chat_server_url_(chat_server_url) {
    http_requester_ = make_unique<HttpRequester>(
        web::http::uri_builder(chat_server_url).to_uri().to_string());
    chat_client_view_ = make_unique<ChatClientView>();
  }

  void ChatClient::RunChatClient() {
    while (true) {
      string_t method;
      if (session_id_.empty() && current_client_status_ == kBeforeLogin) {
        // Services before login: login, signup
        if(!BeforeLogin()) {
          break;
        }
      } else if (!session_id_.empty() && 
                 current_client_status_ == kAfterLogin) {
        // Services after login.
        // 1) get room list, 2) join chat room, 3) create chat room, 4) logout
        if(!AfterLogin()) {
          break;
        }
      } else if (!session_id_.empty() &&
                 !current_chat_room_.empty() && 
                 current_client_status_ == kInChatRoom) {
        // Enter a chat room.
        InChatRoom();
      } else {
        error("Invalid client status: {}", current_client_status_);
        break;
      }
    }
  }

  bool ChatClient::BeforeLogin() {
    const string_t method = chat_client_view_->
        GetUserInput(UU("Enter method name (login, signup, exit): "));

    if (method == UU("signup")) {
      SignUp();
    } else if (method == UU("login")) {
      Login();
    } else if (method == UU("exit")) {
      return false;
    } else {
      ostringstream_t buf;
      buf << method << " not understood.";
      chat_client_view_->DisplayMessage(buf.str());
    }
    return true;
  }

  bool ChatClient::AfterLogin() {
    const string_t method = chat_client_view_->
        GetUserInput(
            UU("Enter method name (join, room_list, room_create, logout, exit): "));

    if (method == UU("room_list")) {
      if (DisplayChatRoomList() == status_codes::Forbidden) {
        ClearUserInformation();
      }
    } else if (method == UU("join")) {
      JoinChatRoom();
    } else if (method == UU("logout")) {
      Logout();
    } else if (method == UU("room_create")) {
      const status_code response_code = CreateChatRoom();
      if (response_code == status_codes::Forbidden) {
        ClearUserInformation();
      } else if (response_code == status_codes::BadRequest) {
        return true;
      }
    } else if (method == UU("exit")) {
      return false;
    } else {
      ostringstream_t buf;
      buf << method << " not understood.";
      chat_client_view_->DisplayMessage(buf.str());
    }
    return true;
  }

  void ChatClient::InChatRoom() {
    chat_room_ = make_unique<ChatRoom>(chat_server_url_,
                                       session_id_,
                                       current_chat_room_);
    chat_room_->RunChatRoom();
    current_client_status_ = kAfterLogin;
  }

  void ChatClient::SignUp() const {
    // User input: ID, password, password once more
    const string_t id = 
        chat_client_view_->GetUserInput(UU("Enter id: "));
    const string_t password =
        chat_client_view_->GetUserInput(UU("Enter password: "));
    const string_t password_again =
        chat_client_view_->GetUserInput(UU("Enter the password once more: "));

    // Check password matching
    if (password != password_again) {
      chat_client_view_->DisplayMessage(UU("Password not matching."));
      return;
    }

    // Check password length
    if (password.size() < 8) {
      chat_client_view_->
          DisplayMessage(UU("Password must be at least 8 characters."));
      return;
    }

    // Make request URL
    ostringstream_t http_request_url;
    http_request_url << "account" << UU("?id=") << id << UU("&password=")
                     << http_requester_->HashString(password);

    // Make HTTP request
    if (http_requester_->MakeHttpRequest(methods::POST, 
                                         http_request_url.str())) {

      chat_client_view_->
          DisplayMessage(UU("Success to signup."));
    }
  }

  void ChatClient::Login() {
    // User input: ID and password
    const string_t id =
        chat_client_view_->GetUserInput(UU("Enter id: "));
    const string_t password =
        chat_client_view_->GetUserInput(UU("Enter password: "));

    // Make request URL
    const string_t nonce = http_requester_->GenerateNonce();
    ostringstream_t http_request_url;
    http_request_url << "login" << UU("?id=") << id << UU("&nonce=") << nonce
                     << UU("&password=")
                     << http_requester_->HashString(
                            http_requester_->HashString(password) + nonce);

    // Make HTTP request
    const http_response response = http_requester_->MakeHttpRequestForResponse(
        methods::POST, http_request_url.str());

    // When the HTTP request succeeds, store session ID,
    // change client status to KAfterLogin
    if (response.status_code() == status_codes::OK) {
      chat_client_view_->DisplayMessage(UU("Success to login."));
      value object = response.extract_json().get();
      session_id_ = object[UU("session_id")].as_string();
      user_id_ = id;
      current_client_status_ = kAfterLogin;
    } 
  }

  status_code ChatClient::DisplayChatRoomList() const {
    // Make request URL
    ostringstream_t http_request_url;
    http_request_url << "chatroom" << UU("?session_id=") << session_id_;

    // When the HTTP request succeeds, print chat room list
    const http_response response = http_requester_->MakeHttpRequestForResponse(
        methods::GET, http_request_url.str());
    const status_code response_code = response.status_code();
    if (response_code == status_codes::OK) {
      // Print chat room list from the Http response.
      const value object = response.extract_json().get();
      chat_client_view_->DisplayChatRoomList(object.as_array());
    }
    return response_code;
  }

  status_code ChatClient::CreateChatRoom() const {
    // User input: chat room
    const string_t chat_room = 
        chat_client_view_->GetUserInput(UU("Enter chat room name: "));


    // Check whether the given chat room exists
    if (IsExistingChatRoom(chat_room)) {
      ostringstream_t buf;
      buf << "Given chat room: " << chat_room << " already exists.";
      chat_client_view_->DisplayMessage(buf.str());
      return status_codes::BadRequest;
    }

    // Make request URL
    ostringstream_t http_request_url;
    http_request_url << "chatroom" << UU("?session_id=")
                     << session_id_ << UU("&chat_room=") << chat_room;

    const status_code response_code = 
        http_requester_->MakeHttpRequest(methods::POST, http_request_url.str());
    if (response_code == status_codes::OK) {
      ostringstream_t buf;
      buf << "Create chat room: " << chat_room;
      chat_client_view_->DisplayMessage(buf.str());
    }
    return response_code;
  }

  void ChatClient::JoinChatRoom() {
    // User input: chat room
    const string_t chat_room =
        chat_client_view_->GetUserInput(UU("Enter chat room name: "));

    // Check whether the given chat room exists
    if (!IsExistingChatRoom(chat_room)) {
      ostringstream_t buf;
      buf << "The chat room not exist: " << current_chat_room_;
      chat_client_view_->DisplayMessage(buf.str());
      current_chat_room_ = UU("");
    } else {
      ostringstream_t buf;
      buf << "Success to join: " << current_chat_room_;
      chat_client_view_->DisplayMessage(buf.str());
      current_client_status_ = kInChatRoom;
      current_chat_room_ = chat_room;
    }
  }

  void ChatClient::Logout() {
    // Make request URL
    ostringstream_t http_request_url;
    http_request_url << "session" << UU("?session_id=") << session_id_;

    const status_code response_code = http_requester_->MakeHttpRequest(
        methods::DEL,
        http_request_url.str());
    if (response_code == status_codes::OK) {
      chat_client_view_->DisplayMessage(UU("Logout succeeds."));
      ClearUserInformation();
    } else if(response_code == status_codes::Forbidden) {
      // The session is invalid: go back to the status before login.
      chat_client_view_->DisplayMessage(UU("The session is invalid."));
      ClearUserInformation();
    } else {
      chat_client_view_->DisplayMessage(UU("Logout failed."));
    }
  }

  bool ChatClient::IsExistingChatRoom(const string_t chat_room) const {
    // Make request URL
    ostringstream_t http_request_url;
    http_request_url << "chatroom" << UU("?session_id=") << session_id_;

    const http_response response = 
        http_requester_->MakeHttpRequestForResponse(methods::GET, 
                                                    http_request_url.str());
    if (response.status_code() == status_codes::OK) {
      // Check whether the given chat room exists
      const value object = response.extract_json().get();
      web::json::array array = object.as_array();
      bool isExist = false;
      for (auto& room : array) {
        if (to_string_t(room[UU("room")].as_string()).compare(chat_room) == 0) {
          isExist = true;
          break;
        }
      }
      return isExist;
    } 
    return false;
  }

  void ChatClient::ClearUserInformation() {
    session_id_.clear();
    user_id_.clear();
    current_chat_room_.clear();
    current_client_status_ = kBeforeLogin;
  }

} // namespace chatclient