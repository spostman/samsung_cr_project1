// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#include "chat_client.h"

using namespace chatclient;
using namespace std;
using ::web::http::http_response;
using ::utility::string_t;

int main(int argc, char* argv[]) {
  const string_t port = UU("34568");
  
  string_t address = UU("http://localhost:");
  address.append(port);

  web::uri_builder uri(address);
  uri.append_path(UU("chat"));

  const string_t chat_server_address = uri.to_uri().to_string();

  ChatClient chat_client(chat_server_address);
  chat_client.RunChatClient();
	return 0;
}