// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#ifndef CHATSERVER_ACCOUNTDATABASE_H_
#define CHATSERVER_ACCOUNTDATABASE_H_

#include <map>
#include <string>

#include "cpprest/json.h"

namespace chatserver {

  // This class is designed to manage pairs of chat ID and password accounts.
  // It use file database for storing ID and password.
  // Example:
  //   AccountDatabase account_database;
  //   account_database.Initialize("account_db.txt");
  //   auto login_result = account_database.Login(id, password);
  //   if(login_result == kLoginSuccess) {
  //     do something to success login
  //   } else if (login_result == kIDNotExist) {
  //     do something to fail login with a non-registered id
  //   } else if (login_result == kPasswordError) {
  //     do something to fail login with a password error
  //   }
  // The usage with SignUp and SignUpResult function are similar to the above.

  class AccountDatabase {
  public:
    // Return values of Login function.
    typedef enum {
      kLoginSuccess,
      kIDNotExist,
      kPasswordError
    } LoginResult;

    // Return values of SignUp function.
    typedef enum {
      kSignUpSuccess,
      kProhibitedCharInID,  // Parsing delimiter is prohibited in ID.
      kDuplicateID,
      kAccountWriteError
    } SignUpResult;

    // Read IDs and passwords from the given file into database.
    bool Initialize(const utility::string_t account_file);
    // Check if there is a given ID and password in the database.
    LoginResult Login(const utility::string_t id,
                      const utility::string_t password);
    // Create a chat account on the database 
    SignUpResult SignUp(const utility::string_t id,
                        const utility::string_t password);
    // Delimiter between id and password in a file database.
    const utility::string_t kParsingDelimeter = UU(",");

  private:
    // Read the given database file
    bool ReadAccountFile(const utility::string_t account_file);
    // Parse file database. Parsing format: id,pw.
    bool ParseAccountFile(std::wifstream account_file);
    // Save account information to account database and file.
    bool StoreAccountInformation(const utility::string_t id,
                                 const utility::string_t password);
    // Check given ID exists on database.
    bool IsExistAccount(const utility::string_t id);

    // Account database: std::map<ID, pwd>
    std::map<utility::string_t, utility::string_t> accounts_;
    // File database name.
    utility::string_t account_file_;
  };
} // namespace chatserver
#endif CHATSERVER_ACCOUNTDATABASE_H_ // CHATSERVER_ACCOUNTDATABASE_H_
