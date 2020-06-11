// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#include "gtest/gtest.h"
#include "account_database.h"
#include "chat_database.h"

using namespace std;
using namespace utility;
using namespace chatserver;
using ::utility::conversions::to_utf8string;

// fixture class for testing
class AccountDatabaseTest : public ::testing::Test {
 protected:
  AccountDatabase account_database_;
  ChatDatabase chat_database_;

  // Delimiter between id and password in a file database.
  string_t kParsingDelimeterAccount = UU(",");
  // Delimiter in the chat message file database.
  string_t kParsingDelimeterChatDb = UU("|");
  // Length of nonce.
  const size_t kNonceLength = 10;
  // Nonce seed.
  const string_t kNonceValue = UU(
      "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

  void SetUp() override {
    const string_t file_name = UU("accounts.txt");
    wofstream file(file_name, wofstream::out | ofstream::trunc);
    file << "kaist" << "," << HashString(UU("12345678")) << endl;
    file << "wsp" << "," << HashString(UU("abcdefgh")) << endl;
    file << "gsis" << "," << HashString(UU("!@#$%^&*")) << endl;
    file.close();
    EXPECT_EQ(true, account_database_.Initialize(file_name));
  }

  string_t HashLoginPassword(string_t password, string_t nonce) const {
    return HashString(HashString(password) + nonce);
  }

  string_t HashString(utility::string_t string) const {
    return conversions::to_string_t(
      std::to_string(std::hash<utility::string_t>{}(string)));
  }

  string_t GenerateNonce() const {
    std::random_device device;
    std::mt19937 generator(device());
    const std::uniform_int_distribution<int>
        distribution(0, kNonceValue.size() - 1);
    string_t result;
    for (size_t i = 0; i < 10; i++) {
      result += kNonceValue.at(distribution(generator));
    }
    return result;
  }
};

TEST_F(AccountDatabaseTest, Initialization_Fail) {
  // Fail to initialization with file does not exist.
  EXPECT_EQ(false, account_database_.Initialize(UU("abcdedef.txt")));
}

TEST_F(AccountDatabaseTest, Login_Success) {
  // Login success.
  string_t nonce = GenerateNonce();
  EXPECT_EQ(AccountDatabase::kAuthSuccess,
            account_database_.Login(UU("kaist"), 
            HashLoginPassword(UU("12345678"), nonce), 
            nonce));
  nonce = GenerateNonce();
  EXPECT_EQ(AccountDatabase::kAuthSuccess,
            account_database_.Login(UU("wsp"), 
            HashLoginPassword(UU("abcdefgh"), nonce), 
            nonce));
  nonce = GenerateNonce();
  EXPECT_EQ(AccountDatabase::kAuthSuccess,
            account_database_.Login(UU("gsis"), 
            HashLoginPassword(UU("!@#$%^&*"), nonce), 
            nonce));
}

TEST_F(AccountDatabaseTest, Login_Fail_Password) {
  // Login fails due to incorrect password.
  const string_t nonce = GenerateNonce();
  EXPECT_EQ(AccountDatabase::kPasswordError,
            account_database_.Login(UU("kaist"), 
            HashLoginPassword(UU("23456789"), nonce), 
            nonce));
}

TEST_F(AccountDatabaseTest, Login_Fail_Id) {
  // Login fails due to incorrect ID.
  const string_t nonce = GenerateNonce();
  EXPECT_EQ(AccountDatabase::kIDNotExist,
            account_database_.Login(UU("gss"), 
            HashLoginPassword(UU("23456789"), nonce), 
            nonce));
}

TEST_F(AccountDatabaseTest, SignUp_Success) {
  // SignUp success.
  EXPECT_EQ(AccountDatabase::kAuthSuccess,
            account_database_.SignUp(UU("abc"), HashString(UU("12345678"))));
  EXPECT_EQ(AccountDatabase::kDuplicateID,
            account_database_.SignUp(UU("wsp"), HashString(UU("45678901"))));
  EXPECT_EQ(AccountDatabase::kDuplicateID,
            account_database_.SignUp(UU("abc"), HashString(UU("78901234"))));
}

TEST_F(AccountDatabaseTest, SignUp_Fail_ProhibitedCharInId) {
  // SignUp fails due to prohibited char.
  EXPECT_EQ(AccountDatabase::kProhibitedCharInID,
      account_database_.SignUp(UU("kaist") + kParsingDelimeterAccount,
          HashString(UU("12345678"))));
  EXPECT_EQ(AccountDatabase::kProhibitedCharInID,
      account_database_.SignUp(UU("kaist") + kParsingDelimeterChatDb,
          HashString(UU("12345678"))));
}

TEST_F(AccountDatabaseTest, SignUp_Fail_ProhibitedCharInPassword) {
  // SignUp fails due to prohibited char.
  EXPECT_EQ(AccountDatabase::kProhibitedCharInPassword,
      account_database_.SignUp(UU("kaist"),
          UU("234") + kParsingDelimeterAccount));
}

TEST_F(AccountDatabaseTest, ParsingAccountFile_Success) {
  AccountDatabase account_database;
  const string_t file_name = UU("accounts.txt");
  wofstream file(file_name, wofstream::out | ofstream::trunc);

  // Parsing success.
  file << "kaist" << "," << "123" << endl;
  file << "wsp" << "," << "456" << endl;
  file.close();
  EXPECT_EQ(true, account_database.Initialize(file_name));
}

TEST_F(AccountDatabaseTest, ParsingAccountFile_Fail_Two_Delimiter) {
  AccountDatabase account_database;
  const string_t file_name = UU("accounts.txt");
  wofstream file(file_name, wofstream::out | ofstream::trunc);

  file << "kaist" << "," << "123" << endl;
  file << "ws,p" << "," << "456" << endl;
  file.close();
  EXPECT_EQ(false, account_database.Initialize(file_name));
}

TEST_F(AccountDatabaseTest, ParsingAccountFile_Fail_NoChar_AfterDelimiter) {
  AccountDatabase account_database;
  const string_t file_name = UU("accounts.txt");
  wofstream file(file_name, wofstream::out | ofstream::trunc);

  file << "kaist" << "," << "123" << endl;
  file << "wsp" << "456," << endl;
  file.close();
  EXPECT_EQ(false, account_database.Initialize(file_name));
}

TEST_F(AccountDatabaseTest, ParsingAccountFile_Fail_NoChar_BeforeDelimiter) {
  AccountDatabase account_database;
  const string_t file_name = UU("accounts.txt");
  wofstream file(file_name, wofstream::out | ofstream::trunc);

  file << "kaist" << "," << "123" << endl;
  file << ",wsp" << "456" << endl;
  file.close();
  EXPECT_EQ(false, account_database.Initialize(file_name));
}

TEST_F(AccountDatabaseTest, ParsingAccountFile_Fail_Incorrect_Delimiter) {
  AccountDatabase account_database;
  const string_t file_name = UU("accounts.txt");
  wofstream file(file_name, wofstream::out | ofstream::trunc);

  file << "kaist" << "," << "123" << endl;
  file << "wsp" << "|" << "456" << endl;
  file.close();
  EXPECT_EQ(false, account_database.Initialize(file_name));
}

TEST_F(AccountDatabaseTest, ParsingAccountFile_Fail_Delimiter_Only) {
  AccountDatabase account_database;
  const string_t file_name = UU("accounts.txt");
  wofstream file(file_name, wofstream::out | ofstream::trunc);

  file << ",,,,,,,,,,," << endl;
  file.close();
  EXPECT_EQ(false, account_database.Initialize(file_name));
}

TEST_F(AccountDatabaseTest, ParsingAccountFileFail_Fail_One_Delimiter) {
  AccountDatabase account_database;
  const string_t file_name = UU("accounts.txt");
  wofstream file(file_name, wofstream::out | ofstream::trunc);

  file << "," << endl;
  file.close();
  EXPECT_EQ(false, account_database.Initialize(file_name));
}

TEST_F(AccountDatabaseTest, ParsingAccountFile_Fail_NoInfo_AfterDelimiter) {
  AccountDatabase account_database;
  const string_t file_name = UU("accounts.txt");
  wofstream file(file_name, wofstream::out | ofstream::trunc);

  file << "a," << endl;
  file.close();
  EXPECT_EQ(false, account_database.Initialize(file_name));
}

TEST_F(AccountDatabaseTest, ParsingAccountFile_Fail_NoInfo_BeforeDelimiter) {
  AccountDatabase account_database;
  const string_t file_name = UU("accounts.txt");
  wofstream file(file_name, wofstream::out | ofstream::trunc);

  file << ",a" << endl;
  file.close();
  EXPECT_EQ(false, account_database.Initialize(file_name));
}