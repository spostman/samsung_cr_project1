// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#include <iostream>
#include <string>

#include "cpprest/http_listener.h"
#include "cpprest/uri.h"
#include "chat_server.h"
#include "spdlog/spdlog.h"

using namespace std;
using ::utility::string_t;
using ::utility::conversions::to_utf8string;
using ::spdlog::info;
using ::spdlog::error;
using ::web::uri_builder;
using ::concurrency::task_status;

namespace chatserver {
  
  int RunChatserver(string_t chat_server_uri) { 
    unique_ptr<ChatDatabase> chat_database = make_unique<ChatDatabase>();
    if (!chat_database->Initialize(UU("chat_messages_sample.txt"),
                                   UU("chat_rooms_sample.txt"))) {
      error("Fail chat database initialization");
      return 0;
    }

    unique_ptr<AccountDatabase> acct_database = make_unique<AccountDatabase>();
    if (!acct_database->Initialize(UU("chat_accounts_sample.txt"))) {
      error("Fail account database initialization");
      return 0;
    }

    unique_ptr<SessionManager> session_manager = make_unique<SessionManager>();

    ChatServer chat_server(chat_database.get(), 
                           acct_database.get(), 
                           session_manager.get());
    if (!chat_server.Initialize(chat_server_uri)) {
      error("Fail chat server initialization");
      return 0;
    }

    // Open the chat server
    task_status status = chat_server.OpenServer().wait();  
    if (status == task_status::completed) {
      info("Complete to open the chat server.");
      info("Listening for requests at: {}", to_utf8string(chat_server_uri));
      info("Press ENTER to exit.");

      // Using the blocking function of standard input,
      // wait for the chat server close
      string line;
      getline(std::cin, line);

      status = chat_server.CloseServer().wait(); // close the chat server
      if (status == task_status::completed) {
        info("Complete to close the chat server.");
      } else {
        error("Fail to close the chat server.");
      }
    } else {
      error("Fail to start the chat server.");
    }
    return 0;
  }

}  // namespace chatserver


int main(int argc, char* argv[]) {
  string_t port = UU("34568");
  if (argc == 2) {
    port = utility::conversions::to_string_t(argv[1]);
  }
  
  string_t address = UU("http://localhost:");
  address.append(port);

  uri_builder uri(address);
  uri.append_path(UU("chat"));

  return chatserver::RunChatserver(uri.to_uri().to_string());
}