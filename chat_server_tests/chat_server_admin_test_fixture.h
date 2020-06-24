// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#ifndef CHATSERVERTESTS_FIXTURE_CLASS_H_
#define CHATSERVERTESTS_FIXTURE_CLASS_H_

#include "chat_server.h"
#include "gtest/gtest.h"
#include "cpprest/http_client.h"

namespace chatservertests {

  // Fixture class for the testing chat_server.cc
  class ChatServerAdminTest : public ::testing::Test {
   protected:
    std::unique_ptr<chatserver::ChatServer> chat_server_;
    std::unique_ptr<web::http::client::http_client> http_client_;
    std::unique_ptr<chatserver::SessionManager> session_manager_;

    // Length of nonce.
    const size_t kNonceLength = 10;
    // Nonce seed.
    const utility::string_t kNonceValue = UU(
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    // Session alive time.
    const time_t kSessionAliveTime = 1;

    void SetUp() override {
      // Create chat message file for ChatDatabase class.
      const utility::string_t chat_message_file_name =
          UU("chat_message_test_chat_server.txt");
      std::wofstream file(chat_message_file_name,
          std::wofstream::out | std::ofstream::trunc);
      file << "1583134930" << "|" << "samsung" << "|" << "1" << "|"
           << "hello" << std::endl;
      file << "1583134945" << "|" << "samsung" << "|" << "2" << "|"
           << "hello~!" << std::endl;
      file.close();

      // Create chat room file for ChatDatabase class.
      const utility::string_t chat_room_file_name =
          UU("chat_room_test_chat_server.txt");
      file.open(chat_room_file_name,
          std::wofstream::out | std::ofstream::trunc);
      file << "1" << std::endl;
      file << "2" << std::endl;
      file.close();

      chat_database_ = std::make_unique<chatserver::ChatDatabase>();
      chat_database_->Initialize(UU("chat_message_test_chat_server.txt"),
                                 UU("chat_room_test_chat_server.txt"));

      // Setup account file.
      const utility::string_t account_file_name =
          UU("accounts_test_chat_server.txt");
      file.open(account_file_name, std::wofstream::out | std::ofstream::trunc);
      file << "samsung" << "," << HashString(UU("12345678")) << std::endl;
      file << "galaxy" << "," << HashString(UU("abcdefgh")) << std::endl;
      file.close();

      account_database_ = std::make_unique<chatserver::AccountDatabase>();
      account_database_->Initialize(UU("accounts_test_chat_server.txt"));

      session_manager_ = 
          std::make_unique<chatserver::SessionManager>(kSessionAliveTime);

      // Start the chat server.
      chat_server_ =
          std::make_unique<chatserver::ChatServer>(chat_database_.get(),
          account_database_.get(),
          session_manager_.get());
      utility::string_t address = UU("http://localhost:34568/chat");
      chat_server_->Initialize(address);
      concurrency::task_status::completed, chat_server_->OpenServer().wait();

      // Make HTTP client for making a request to chat server.
      http_client_ = std::make_unique<web::http::client::http_client>(address);
    }

    void TearDown() override {
      concurrency::task_status::completed, chat_server_->CloseServer().wait();
      chat_server_.reset();
      remove("accounts_test_chat_server.txt");
      remove("chat_message_test_chat_server.txt");
      remove("chat_room_test_chat_server.txt");
    }

    utility::string_t HashLoginPassword(utility::string_t password,
      utility::string_t nonce) const {
      return HashString(HashString(password) + nonce);
    }

    utility::string_t HashString(utility::string_t string) const {
      return utility::conversions::to_string_t(
          std::to_string(std::hash<utility::string_t>{}(string)));
    }

    utility::string_t GenerateNonce() const {
      std::random_device device;
      std::mt19937 generator(device());
      std::uniform_int_distribution<int> distribution(0, 
                                                      kNonceValue.size() - 1);
      utility::string_t result;
      for (size_t i = 0; i < 10; i++) {
        result += kNonceValue.at(distribution(generator));
      }
      return result;
    }

    utility::string_t PerformSuccessfulLogin() {
      // Login process for obtaining session ID
      utility::ostringstream_t buf;
      const utility::string_t nonce = GenerateNonce();
      buf << "login";
      web::json::value body_data;
      body_data[UU("id")] = web::json::value::string(UU("samsung"));
      body_data[UU("nonce")] = web::json::value::string(nonce);
      body_data[UU("password")] =
          web::json::value::string(HashLoginPassword(UU("12345678"), nonce));
      web::http::http_response response = http_client_->request(
          web::http::methods::POST,
          web::uri::encode_uri(buf.str()),
          body_data).get();
      web::json::value object = response.extract_json().get();
      return object[UU("session_id")].as_string();
    }

   private:
    std::unique_ptr<chatserver::ChatDatabase> chat_database_;
    std::unique_ptr<chatserver::AccountDatabase> account_database_;
  };

} // namespace chatservertests

#endif CHATSERVERTESTS_FIXTURE_CLASS_H_ // CHATSERVERTESTS_FIXTURE_CLASS_H_