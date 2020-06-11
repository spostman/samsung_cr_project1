// Code review content development project.
// Code style follows Google C++ Style Guide.
// (https://google.github.io/styleguide/cppguide.html)

#ifndef CHATSERVER_ACCOUNTDATABASE_H_
#define CHATSERVER_ACCOUNTDATABASE_H_

#include <map>
#include <string>

#include "cpprest/json.h"

// This class is designed to manage pairs of chat ID and password accounts.
// It uses a file database that holds IDs and passwords.
// Example:
//   AccountDatabase account_database;
//   account_database.Initialize("account_db.txt");
//   AuthResult login_result = account_database.Login(id, password);
//   if (login_result == kAuthSuccess) {
//     do something after login success.
//   } else if (login_result == kIDNotExist) {
//     do something with a non-registered id.
//   } else if (login_result == kPasswordError) {
//     do something with failed login attempts with wrong passwords.
//   }
// The usage of the AuthResult functions is similar to the above.

namespace chatserver {

  class AccountDatabase {
   public:
    // Return values of SignUp and Login function.
    typedef enum {
      kAuthSuccess,
      // Prohibited parsing delimiter is in ID.
      kProhibitedCharInID,  
      // Prohibited parsing delimiter is in password.
      kProhibitedCharInPassword,  
      kDuplicateID,
      kAccountWriteError,
      kIDNotExist,
      kPasswordError
    } AuthResult;

    // Read IDs and passwords from the given file into database.
    bool Initialize(utility::string_t account_file);

    // Check if there is a given ID and password in the database.
    AuthResult Login(utility::string_t id,
                      utility::string_t password,
                      utility::string_t nonce);

    // Create a chat account on the database 
    AuthResult SignUp(utility::string_t id,
                        utility::string_t password);

   private:
    // Read the given database file
    bool ReadAccountFile(utility::string_t account_file);

    // Parse file database. Parsing format: id, hash(pwd).
    bool ParseAccountFile(std::wifstream account_file);

    // Save account information to account database and file.
    bool StoreAccountInformation(utility::string_t id,
                                 utility::string_t password);

    // Check given ID exists on database.
    bool IsExistAccount(utility::string_t id) const;

    // Hash string.
    utility::string_t HashString(utility::string_t string) const;

    // Account database: std::map<ID, pwd>
    std::map<utility::string_t, utility::string_t> accounts_;

    // File database name.
    utility::string_t account_file_;
  };

} // namespace chatserver

#endif CHATSERVER_ACCOUNTDATABASE_H_ // CHATSERVER_ACCOUNTDATABASE_H_
