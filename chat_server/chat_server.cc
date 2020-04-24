#include "chat_server.h"

#include "spdlog/spdlog.h"

using namespace std;
using namespace pplx;
using ::utility::string_t;
using ::utility::conversions::to_utf8string;
using namespace web::http;
using namespace web::json;
using namespace web::http::experimental::listener;
using ::spdlog::info;

namespace chatserver {

	bool ChatServer::Initialize(const string_t server_url,
                              const string_t chat_message_file_path,
                              const string_t chat_room_file_path, 
                              const string_t chat_account_file_path) {
    listener_ = http_listener(server_url);
		listener_.support(methods::GET,
		                   bind(&ChatServer::HandleGet, this,
		                             placeholders::_1));
		listener_.support(methods::PUT,
		                   bind(&ChatServer::HandlePut, this,
		                             placeholders::_1));
		listener_.support(methods::POST,
		                   bind(&ChatServer::HandlePost, this,
		                             placeholders::_1));
		listener_.support(methods::DEL,
		                   bind(&ChatServer::HandleDelete, this,
		                             placeholders::_1));

		chat_database_.Initialize(UU("chat_messages.txt"), UU("chat_rooms.txt"));
		account_database_.Initialize(UU("accounts.txt"));
    return true;
	}

  task<void> ChatServer::OpenServer() {
    return listener_.open();
	}

  task<void> ChatServer::CloseServer() {
    return listener_.close();
  }

	void ChatServer::HandleGet(const http_request message) {
    info("Handle get: {}", to_utf8string(message.to_string()));
		auto paths = uri::split_path(
			uri::decode(message.relative_uri().path()));
		auto query = uri::split_query(
			uri::decode(message.relative_uri().query()));

		if (paths.empty()) {
			message.reply(status_codes::NotFound);
			return;
		}

		if (paths[0] == UU("chatmessage")) {
			auto chat_room = query.find(UU("chat_room"));
			if (chat_room == query.end()) {
				message.reply(status_codes::Forbidden,
				              UU("Chat room information absence"));
				return;
			}

      const vector<ChatMessage>& chat_messages = chat_database_.GetAllChatMessages(chat_room->second);
			value result = value::array();
			size_t idx = 0;
			for (auto chat_message = chat_messages.begin(); chat_message !=
			     chat_messages.end(); ++chat_message) {
				value json_obj = value::object();

				json_obj[UU("date")] = value::number(chat_message->date);
				json_obj[UU("user_id")] = value::
					string(chat_message->user_id);
				json_obj[UU("message")] = value::string(
					chat_message->chat_message);
				json_obj[UU("room")] = value::string(chat_message->chat_room);

				result[idx++] = json_obj;
			}

			message.reply(status_codes::OK, result);
			return;
		} else if (paths[0] == UU("chatroomlist")) {
			vector<string_t> chat_room_list = chat_database_.
				GetChatRoomList();

			value result = value::array();
			size_t idx = 0;
			for (auto room = chat_room_list.begin(); room != chat_room_list.end(); ++
			     room) {
				value json_obj = value::object();

				json_obj[UU("room")] = value::string(room->data());
				result[idx++] = json_obj;
			}

			message.reply(status_codes::OK, result);
			return;
		}
	};

	void ChatServer::HandlePost(http_request message) {
    info("Handle post: {}", to_utf8string(message.to_string()));

		auto paths = web::uri::split_path(
			web::uri::decode(message.relative_uri().path()));
		auto query = web::uri::split_query(
			web::uri::decode(message.relative_uri().query()));

		if (paths.empty()) {
			message.reply(status_codes::NotFound);
		}


		if (paths[0] == UU("account")) {
			// create account
			auto id = query.find(UU("id"));
			auto pwd = query.find(UU("pwd"));
			if (id == query.end() || pwd == query.end()) {
				message.reply(status_codes::Forbidden,
				              UU("Account information absence"));
				return;
			}

			int signup_result = account_database_.SignUp(id->second, pwd->second);
			if (signup_result == AccountDatabase::kSignUpSuccess) {
				message.reply(status_codes::OK);
				return;
			} else if (signup_result == AccountDatabase::kProhibitedCharInID) {
				message.reply(status_codes::Forbidden,
				              UU("Prohibited character in ID"));
				return;
			} else if (signup_result == AccountDatabase::kDuplicateID) {
				message.reply(status_codes::Forbidden, UU("Duplicated ID"));
				return;
			} else if (signup_result == AccountDatabase::kAccountWriteError) {
				message.reply(status_codes::Forbidden,
				              UU("Account write error in file DB"));
				return;
			}
		} else if (paths[0] == UU("login")) {
			// login
			auto id = query.find(UU("id"));
			auto pwd = query.find(UU("pwd"));
			if (id == query.end() || pwd == query.end()) {
				message.reply(status_codes::Forbidden,
				              UU("Account information absence"));
				return;
			}

			int login_result = account_database_.Login(id->second, pwd->second);
			if (login_result == AccountDatabase::kLoginSuccess) {
				Session session = session_manager_.CreateSession(id->second);
				value result = value::object();
				//result[STATUS] = value::number((double)Status);
				result[UU("session_id")] = value::string(session.session_id);
				message.reply(status_codes::OK, result);
				return;
			} else if (login_result == AccountDatabase::kIDNotExist) {
				message.reply(status_codes::Forbidden, UU("ID not exist"));
				return;
			} else if (login_result == AccountDatabase::kPasswordError) {
				message.reply(status_codes::Forbidden,
				              UU("Passwrod not matched"));
				return;
			}
		}

		auto session_id = query.find(UU("session_id"));
		if (session_id == query.end() || !IsValidSession(session_id->second)) {
			message.reply(status_codes::Forbidden,
			              UU("Not a vaild session"));
			return;
		}

		if (paths[0] == UU("chat")) {
			if (paths[1] == UU("message")) {

				if (query.find(UU("chat_message")) == query.end() || query.
					find(UU("chat_room")) == query.end()) {
					message.reply(status_codes::BadRequest,
					              UU("Chat message or chat room name not exist"));
					return;
				}

				ChatMessage chat_message;
        //deal with if false
				session_manager_.GetUserIDFromSessionID(
            session_id->second, &chat_message.user_id);
				chat_message.chat_message = query.find(UU("chat_message"))->second;
				chat_message.chat_room = query.find(UU("chat_room"))->second;
				chat_message.date = chrono::system_clock::to_time_t(
					chrono::system_clock::now());
				auto result = chat_database_.StoreChatMessage(chat_message);

				if (result) {
					message.reply(status_codes::OK);
				} else {
					message.reply(status_codes::NotFound,
					              UU("Fail to store chat message"));
				}
				return;
			} else if (paths[1] == UU("room")) {
				if (query.find(UU("chat_room")) == query.end()) {
					message.reply(status_codes::NotFound,
					              UU("Chat room name not exist"));
					return;
				}

				chat_database_.CreateChatRoom(query.find(UU("chat_room"))->second);
				message.reply(status_codes::OK);
				return;
			}
		}

		if (paths.empty()) {

		}
	};

  void ChatServer::HandleDelete(http_request message) {
    info("Handle delete: {}", to_utf8string(message.to_string()));

		auto paths = web::uri::split_path(
			web::uri::decode(message.relative_uri().path()));
		auto query = web::uri::split_query(
			web::uri::decode(message.relative_uri().query()));

		if (paths[0] == UU("session")) {
			auto session_id = query.find(UU("session_id"));
			auto result = session_manager_.DeleteSession(session_id->second);
			if (result) {
				message.reply(status_codes::OK);
			} else {
				message.reply(status_codes::NotFound);
			}
		}
	};

	void ChatServer::HandlePut(const http_request message) {
    info("Handle put: {}", to_utf8string(message.to_string()));

		auto paths = web::uri::split_path(
			web::uri::decode(message.relative_uri().path()));
		auto query = web::uri::split_query(
			web::uri::decode(message.relative_uri().query()));
	};

	bool ChatServer::IsValidSession(string_t session_id) {
		return session_manager_.IsExistSessionID(session_id);
	}
} // namespace chatserver
