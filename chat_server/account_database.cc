#include "account_database.h"

#include "spdlog/spdlog.h"

namespace chatserver {

  using namespace std;
  using ::utility::string_t;
  using ::utility::conversions::to_utf8string;
  using ::spdlog::error;

  bool AccountDatabase::Initialize(const string_t account_file) {
    account_file_ = account_file;
    if (!ReadAccountFile(account_file_)) {
      error("Error to open account file: {}", to_utf8string(account_file_));
      return false;
    }
    return true;
  }

  AccountDatabase::LoginResult AccountDatabase::Login(
    const string_t id, const string_t password) {
    if (!IsExistAccount(id)) {
      return kIDNotExist;
    } else if (accounts_[id] == password) {
      return kLoginSuccess;
    } else {
      return kPasswordError;
    }
  }

  AccountDatabase::SignUpResult AccountDatabase::SignUp(
    const string_t id, const string_t password) {
    if (id.find(kParsingDelimeter) != string_t::npos) {
      return kProhibitedCharInID;
    } else if (IsExistAccount(id)) {
      return kDuplicateID;
    } else if (!StoreAccountInformation(id, password)) {
      return kAccountWriteError;
    } else {
      return kSignUpSuccess;
    }
  }

  bool AccountDatabase::ReadAccountFile(const string_t account_file) {
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
      const int index = line.find(kParsingDelimeter);

      if (index == 0 ||
          index == line.length() - 1 ||
          index == string_t::npos ||
          line.find(kParsingDelimeter, index + 1) != string_t::npos) {
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

  bool AccountDatabase::StoreAccountInformation(const string_t id,
                                                const string_t password) {
    // If the file exists, work with it, if no, create it
    wofstream file(account_file_, wofstream::out | wofstream::app);
    if (file.is_open()) {
      file << id << kParsingDelimeter << password << endl;
      file.close();
      accounts_[id] = password;
    } else {
      error("Can't open account file");
      return false;
    }
    return true;
  }

  bool AccountDatabase::IsExistAccount(const string_t id) {
    if (accounts_.find(id) == accounts_.end()) {
      return false;
    } else {
      return true;
    }
  }
} // namespace chatserver
