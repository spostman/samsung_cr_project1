#ifndef CHATSERVER_CHATSERVER_H_
#define CHATSERVER_CHATSERVER_H_

#include "cpprest/asyncrt_utils.h"
#include "cpprest/http_listener.h"
#include "cpprest/json.h"
#include "cpprest/uri.h"
#include "cpprest/details/basic_types.h"
#include <string>
#include <vector>
#include <map>

#include "account_database.h"
#include "chat_message.h"
#include "session_manager.h"
#include "chat_database.h"

namespace chatserver {
	class ChatServer {
	public:
    bool Initialize(const utility::string_t server_url,
                    const utility::string_t chat_message_file_path,
                    const utility::string_t chat_room_file_path,
                    const utility::string_t chat_account_file_path);
    pplx::task<void> OpenServer();
    pplx::task<void> CloseServer();

	private:
		void HandleGet(web::http::http_request message);
		void HandlePut(web::http::http_request message);
		void HandlePost(web::http::http_request message);
		void HandleDelete(web::http::http_request message);
		bool IsValidSession(utility::string_t session_id);

		web::http::experimental::listener::http_listener listener_;
		std::vector<ChatMessage> chat_messages_;
		std::map<utility::string_t, int> chat_rooms_;
		ChatDatabase chat_database_;
		AccountDatabase account_database_;
		SessionManager session_manager_;
	};
} // namespace chatserver
#endif CHATSERVER_CHATSERVER_H_ // CHATSERVER_CHATSERVER_H_