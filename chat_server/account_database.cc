// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#include "account_database.h"

#include "spdlog/spdlog.h"
#include "chat_database.h"

using namespace std;
using ::utility::string_t;
using ::utility::conversions::to_string_t;
using ::utility::conversions::to_utf8string;
using ::spdlog::error;

namespace chatserver {

  // Delimiter between id and password in a file database.
  string_t kParsingDelimeterAccount = UU(",");
  // Delimiter in the chat message file database.
  string_t kParsingDelimeterChatDb = UU("|");

  bool AccountDatabase::Initialize(string_t account_file) {
    account_file_ = account_file;
    if (!ReadAccountFile(account_file_)) {
      error("Error to open account file: {}", to_utf8string(account_file_));
      return false;
    }
    return true;
  }

  AccountDatabase::AuthResult AccountDatabase::Login(string_t id, 
                                                      string_t password,
                                                      string_t nonce) {
    if (!IsExistAccount(id)) {
      return kIDNotExist;
    } else if (HashString(accounts_[id] + nonce) == password) {
      return kAuthSuccess;
    } else {
      return kPasswordError;
    }
  }

  AccountDatabase::AuthResult AccountDatabase::SignUp(string_t id,
                                                        string_t password) {
    if (id.find(kParsingDelimeterAccount) != string_t::npos ||
        id.find(kParsingDelimeterChatDb) != string_t::npos) {
      return kProhibitedCharInID;
    } else if (password.find(kParsingDelimeterAccount) != string_t::npos) {
      return kProhibitedCharInPassword;
    } else if (IsExistAccount(id)) {
      return kDuplicateID;
    } else if (!StoreAccountInformation(id, password)) {
      return kAccountWriteError;
    } else {
      return kAuthSuccess;
    }
  }

  bool AccountDatabase::ReadAccountFile(string_t account_file) {
    wifstream file(account_file);
    if (!file.is_open()) {
      error("Can't open account file: {}", to_utf8string(account_file));
      return false;
    }

    if (!ParseAccountFile(move(file))) {
      error("Parsing error: {}", to_utf8string(account_file));
      file.close();
      return false;
    }
    file.close();
    return true;
  }

  bool AccountDatabase::ParseAccountFile(wifstream file) {
    string_t line;
    while (file.good()) {
      getline(file, line);
      if (line.length() == 0) continue;
      const int index = line.find(kParsingDelimeterAccount);

      if (index == 0 ||
          index == line.length() - 1 ||
          index == string_t::npos ||
          line.find(kParsingDelimeterAccount, index + 1) != string_t::npos) {
        error("Account file parsing error");
        accounts_.clear();
        return false;
      }

      const auto account_name = line.substr(0, index);
      const auto password = line.substr(index + 1);
      accounts_[account_name] = password;
    }
    return true;
  }

  bool AccountDatabase::StoreAccountInformation(string_t id,
                                                string_t password) {
    // If the file exists, work with it, if no, create it
    wofstream file(account_file_, wofstream::out | wofstream::app);
    if (file.is_open()) {
      file << id << kParsingDelimeterAccount << password << endl;
      file.close();
      accounts_[id] = password;
    } else {
      error("Can't open account file");
      return false;
    }
    return true;
  }

  bool AccountDatabase::IsExistAccount(string_t id) const {
    if (accounts_.find(id) == accounts_.end()) {
      return false;
    } else {
      return true;
    }
  }

  string_t AccountDatabase::HashString(string_t string) const {
    return to_string_t(to_string(hash<string_t>{}(string)));
  }
} // namespace chatserver
