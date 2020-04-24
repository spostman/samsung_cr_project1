#include <iostream>
#include <string>

#include "chat_server.h"
#include "cpprest/http_listener.h"
#include "cpprest/json.h"
#include "cpprest/uri.h"

using namespace web::json;

std::unique_ptr<chatserver::ChatServer> http_server;

void on_initialize(const utility::string_t& address) {
  web::uri_builder uri(address);
  uri.append_path(UU("chat"));

  utility::string_t addr = uri.to_uri().to_string();
  http_server = std::make_unique<chatserver::ChatServer>();
  http_server->Initialize(addr, 
                          UU("chat_messages.txt"), 
                          UU("chat_rooms.txt"), 
                          UU("accounts.txt"));
  http_server->OpenServer().wait();

  ucout << utility::string_t(UU("Listening for requests at: ")) << addr << std::
    endl;
  return;
}

void on_shutdown() {
  http_server->CloseServer().wait();
  return;
}

int main(int argc, char* argv[]) {
  utility::string_t port = UU("34568");
  if (argc == 2) {
    port = utility::conversions::to_string_t(argv[1]);
  }

  utility::string_t address = UU("http://localhost:");
  address.append(port);

  on_initialize(address);
  std::cout << "Press ENTER to exit." << std::endl;

  std::string line;
  std::getline(std::cin, line);

  on_shutdown();
  return 0;
}
