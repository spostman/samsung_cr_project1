#include "account_database.h"
#include "gtest/gtest.h"

using namespace std;
using namespace utility;
using namespace chatserver;

// fixture class for testing
class AccountDatabaseTest : public ::testing::Test {
protected:
  AccountDatabase account_database_;

  void SetUp() override {
    const string_t file_name = UU("accounts.txt");
    wofstream file(file_name, wofstream::out | ofstream::trunc);
    file << "kaist" << "," << "123" << endl;
    file << "wsp" << "," << "456" << endl;
    file << "gsis" << "," << "789" << endl;
    file.close();
    account_database_.Initialize(file_name);
  }
};

TEST_F(AccountDatabaseTest, Initialization) {
  EXPECT_EQ(false, account_database_.Initialize(UU("abcdedef.txt")));
}

TEST_F(AccountDatabaseTest, Login) {
  EXPECT_EQ(AccountDatabase::kLoginSuccess,
            account_database_.Login(UU("kaist"), UU("123")));
  EXPECT_EQ(AccountDatabase::kLoginSuccess,
            account_database_.Login(UU("wsp"), UU("456")));
  EXPECT_EQ(AccountDatabase::kLoginSuccess,
            account_database_.Login(UU("gsis"), UU("789")));
  EXPECT_EQ(AccountDatabase::kPasswordError,
            account_database_.Login(UU("kaist"), UU("234")));
  EXPECT_EQ(AccountDatabase::kIDNotExist,
            account_database_.Login(UU("gss"), UU("234")));
}

TEST_F(AccountDatabaseTest, SignUp) {
  EXPECT_EQ(AccountDatabase::kSignUpSuccess,
            account_database_.SignUp(UU("abc"), UU("123")));
  EXPECT_EQ(AccountDatabase::kDuplicateID,
            account_database_.SignUp(UU("wsp"), UU("456")));
  EXPECT_EQ(AccountDatabase::kDuplicateID,
            account_database_.SignUp(UU("abc"), UU("789")));
  EXPECT_EQ(AccountDatabase::kProhibitedCharInID,
            account_database_.SignUp(UU("kai,st"), UU("234")));
}

TEST(AccountDatabase, ParsingAccountFile) {
  AccountDatabase account_database;
  const string_t file_name = UU("accounts.txt");
  wofstream file(file_name,
                 wofstream::out | ofstream::trunc);
  file << "kaist" << "," << "123" << endl;
  file << "ws,p" << "," << "456" << endl;
  file.close();
  EXPECT_EQ(false, account_database.Initialize(file_name));

  file.open(file_name,
            wofstream::out | ofstream::trunc);
  file << "kaist" << "," << "123" << endl;
  file << "wsp" << "456," << endl;
  file.close();
  EXPECT_EQ(false, account_database.Initialize(file_name));

  file.open(file_name,
            wofstream::out | ofstream::trunc);
  file << "kaist" << "," << "123" << endl;
  file << ",wsp" << "456" << endl;
  file.close();
  EXPECT_EQ(false, account_database.Initialize(file_name));

  file.open(file_name,
            wofstream::out | ofstream::trunc);
  file << "kaist" << "," << "123" << endl;
  file << "wsp" << "|" << "456" << endl;
  file.close();
  EXPECT_EQ(false, account_database.Initialize(file_name));

  file.open(file_name,
            wofstream::out | ofstream::trunc);
  file << "kaist" << "," << "123" << endl;
  file << "wsp" << "," << "456" << endl;
  file.close();
  EXPECT_EQ(true, account_database.Initialize(file_name));

  file.open(file_name,
            wofstream::out | ofstream::trunc);
  file << ",,,,,,,,,,," << endl;
  file.close();
  EXPECT_EQ(false, account_database.Initialize(file_name));

  file.open(file_name,
            wofstream::out | ofstream::trunc);
  file << "," << endl;
  file.close();
  EXPECT_EQ(false, account_database.Initialize(file_name));

  file.open(file_name,
            wofstream::out | ofstream::trunc);
  file << "a," << endl;
  file.close();
  EXPECT_EQ(false, account_database.Initialize(file_name));

  file.open(file_name,
            wofstream::out | ofstream::trunc);
  file << ",a" << endl;
  file.close();
  EXPECT_EQ(false, account_database.Initialize(file_name));
}
