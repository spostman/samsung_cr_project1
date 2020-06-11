// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#ifndef CHATCLIENTTESTS_FIXTURE_CLASS_H_
#define CHATCLIENTTESTS_FIXTURE_CLASS_H_

#include "chat_client.h"
#include "chat_server.h"
#include "gtest/gtest.h"

namespace chatclient {

  // Fixture class for the testing chat_client.cc
  class ChatClientTest : public ::testing::Test {
  protected:
    std::unique_ptr<chatclient::ChatClient> chat_client_;
    std::unique_ptr<chatclient::HttpRequester> http_requester_;
    std::unique_ptr<chatserver::ChatServer> chat_server_;

    utility::string_t server_address = UU("http://localhost:34568/chat");

    void SetUp() override {
      // Create chat message file for ChatDatabase class
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

      // Create chat room file for ChatDatabase class
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

      // Create account file
      const utility::string_t account_file_name =
          UU("accounts_test_chat_server.txt");
      file.open(account_file_name, std::wofstream::out | std::ofstream::trunc);
      file << "kaist" << ","
           << http_requester_->HashString(UU("12345678")) << std::endl;
      file << "wsp" << ","
           << http_requester_->HashString(UU("abcdefgh")) << std::endl;
      file << "gsis" << ","
           << http_requester_->HashString(UU("!@#$%^&*")) << std::endl;
      file.close();

      account_database_ = std::make_unique<chatserver::AccountDatabase>();
      EXPECT_EQ(
          true,
          account_database_->Initialize(UU("accounts_test_chat_server.txt")));

      session_manager_ = std::make_unique<chatserver::SessionManager>();

      // Start the chat server
      chat_server_ = 
          std::make_unique<chatserver::ChatServer>(chat_database_.get(), 
                                                   account_database_.get(), 
                                                   session_manager_.get());

      EXPECT_EQ(true,
          chat_server_->Initialize(server_address));

      EXPECT_EQ(concurrency::task_status::completed,
          chat_server_->OpenServer().wait());
    }

    void TearDown() override {
      EXPECT_EQ(concurrency::task_status::completed,
          chat_server_->CloseServer().wait());
      chat_server_.reset();
      chat_client_.reset();
      EXPECT_EQ(0, remove("accounts_test_chat_server.txt"));
      EXPECT_EQ(0, remove("chat_message_test_chat_server.txt"));
      EXPECT_EQ(0, remove("chat_room_test_chat_server.txt"));
    }

    std::unique_ptr<chatclient::ChatClient> GetChatClient() const {
      return std::make_unique<chatclient::ChatClient>(server_address);
    }

    utility::string_t GetSessionID() const {
      return chat_client_->session_id_;
    }

    utility::string_t GetUserID() const {
      return chat_client_->user_id_;
    }

    utility::string_t GetCurrentChatRoom() const {
      return chat_client_->current_chat_room_;
    }

    ChatClient::ClientStatus GetClientCurrentStatus() const {
      return chat_client_->current_client_status_;
    }

    // Runs chat client for getting C++ standard output of chat client.
    // Make return value by intercepting C++ standard output of chat client.
    utility::string_t GetStandardOutputFromChatClient(
        std::unique_ptr<chatclient::ChatClient>& chat_client) const {
      return GetStandardOutputFromChatClientWithSleep(
          chat_client, 
          std::chrono::milliseconds(0));
    }

    // This function used for testing ChatRoom.
    // Sleep for waiting for delayed console output caused by an async thread.
    // Runs chat client for getting C++ standard output of chat client.
    // Make return value by intercepting C++ standard output of chat client.
    utility::string_t GetStandardOutputFromChatClientWithSleep(
        std::unique_ptr<chatclient::ChatClient>& chat_client,
        const std::chrono::duration<double> sleep_duration) const {
      // Intercepting C++ standard output.
      testing::internal::CaptureStdout();
      chat_client->RunChatClient();
      std::this_thread::sleep_for(sleep_duration);
      // End for intercepting C++ standard output.
      // Save the output to standard_output
      utility::string_t standard_output =
        utility::conversions::to_string_t(
          testing::internal::GetCapturedStdout());
      return standard_output;
    }

  private:
    std::unique_ptr<chatserver::ChatDatabase> chat_database_;
    std::unique_ptr<chatserver::AccountDatabase> account_database_;
    std::unique_ptr<chatserver::SessionManager> session_manager_;
  };

} // namespace chatclient

#endif CHATCLIENTTESTS_FIXTURE_CLASS_H_ // CHATCLIENTTESTS_FIXTURE_CLASS_H_