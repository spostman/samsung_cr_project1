// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#ifndef CHATSERVERTESTS_FIXTURE_CLASS_H_
#define CHATSERVERTESTS_FIXTURE_CLASS_H_

#include "../chat_client/http_requester.h"
#include "chat_server.h"
#include "gtest/gtest.h"
#include "cpprest/http_client.h"
#include "spdlog/spdlog.h"

namespace chatservertests {

  // Fixture class for the testing chat_server.cc
  class ChatServerTest : public ::testing::Test {
   protected:
    std::unique_ptr<chatserver::ChatServer> chat_server_;
    std::unique_ptr<web::http::client::http_client> http_client_;

    // Delimiter between id and password in a file database.
    utility::string_t kParsingDelimeterAccount = UU(",");
    // Delimiter in the chat message file database.
    utility::string_t kParsingDelimeterChatDb = UU("|");

    // Length of nonce
    const size_t kNonceLength = 10;
    // Nonce seed.
    const utility::string_t kNonceValue = UU(
      "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

    void SetUp() override {
      // Create chat message file for ChatDatabase class.
      const utility::string_t chat_message_file_name = 
          UU("chat_message_test_chat_server.txt");
      std::wofstream file(chat_message_file_name, 
                          std::wofstream::out | std::ofstream::trunc);
      file << "1583134930" << "|" << "kaist" << "|" << "1" << "|"
           << "hello" << std::endl;
      file << "1583134935" << "|" << "wsp" << "|" << "1" << "|"
           << "hi" << std::endl;
      file << "1583134940" << "|" << "gsis" << "|" << "1" << "|"
           << "nice to meet you" << std::endl;
      file << "1583134955" << "|" << "kaist" << "|" << "1" << "|"
           << "hello!!! hihi" << std::endl;
      file << "1583134945" << "|" << "kaist" << "|" << "2" << "|"
           << "hello~!" << std::endl;
      file << "1583134950" << "|" << "wsp" << "|" << "3" << "|"
           << "hello :)" << std::endl;
      file << "1583134955" << "|" << "gsis" << "|" << "abc" << "|"
           << "hello!!!" << std::endl;
      
      file.close();

      // Create chat room file for ChatDatabase class.
      const utility::string_t chat_room_file_name = 
          UU("chat_room_test_chat_server.txt");
      file.open(chat_room_file_name, 
                std::wofstream::out | std::ofstream::trunc);
      file << "1" << std::endl;
      file << "2" << std::endl;
      file << "3" << std::endl;
      file << "abc" << std::endl;
      file.close();

      chat_database_ = std::make_unique<chatserver::ChatDatabase>();
      EXPECT_EQ(
          true, 
          chat_database_->Initialize(UU("chat_message_test_chat_server.txt"),
                                     UU("chat_room_test_chat_server.txt")));

      // Setup account file.
      const utility::string_t account_file_name = 
          UU("accounts_test_chat_server.txt");
      file.open(account_file_name, std::wofstream::out | std::ofstream::trunc);
      file << "kaist" << "," << HashString(UU("12345678")) << std::endl;
      file << "wsp" << "," << HashString(UU("abcdefgh")) << std::endl;
      file << "gsis" << "," << HashString(UU("!@#$%^&*")) << std::endl;
      file.close();

      account_database_ = std::make_unique<chatserver::AccountDatabase>();
      EXPECT_EQ(
          true, 
          account_database_->Initialize(UU("accounts_test_chat_server.txt")));

      session_manager_ = std::make_unique<chatserver::SessionManager>();
        
      // Start the chat server.
      chat_server_ = 
          std::make_unique<chatserver::ChatServer>(chat_database_.get(),
                                                   account_database_.get(),
                                                   session_manager_.get());
      utility::string_t address = UU("http://localhost:34568/chat");

      EXPECT_EQ(true,
                chat_server_->Initialize(address));

      EXPECT_EQ(concurrency::task_status::completed, 
                chat_server_->OpenServer().wait());

      // Make HTTP client for making a request to chat server.
      http_client_ = std::make_unique<web::http::client::http_client>(address);
    }

    void TearDown() override {
      EXPECT_EQ(concurrency::task_status::completed, 
                chat_server_->CloseServer().wait());
      chat_server_.reset();
      EXPECT_EQ(0, remove("accounts_test_chat_server.txt"));
      EXPECT_EQ(0, remove("chat_message_test_chat_server.txt"));
      EXPECT_EQ(0, remove("chat_room_test_chat_server.txt"));
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
      std::uniform_int_distribution<int> distribution(0, kNonceValue.size() - 1);
      utility::string_t result;
      for (size_t i = 0; i < 10; i++) {
        result += kNonceValue.at(distribution(generator));
      }
      return result;
    }

   private:
    std::unique_ptr<chatserver::ChatDatabase> chat_database_;
    std::unique_ptr<chatserver::AccountDatabase> account_database_;
    std::unique_ptr<chatserver::SessionManager> session_manager_;
  };

} // namespace chatservertests

#endif CHATSERVERTESTS_FIXTURE_CLASS_H_ // CHATSERVERTESTS_FIXTURE_CLASS_H_