#ifndef USER_H
#define USER_H

#include "Account.h"

#include <vector>
#include <memory>

class User
{
    friend QDebug &operator<< (QDebug& debug, const std::shared_ptr<User>& obj);

private:
    std::string username;
    std::string password;
    std::vector<std::shared_ptr<Account>> accounts;

public:
    User(const std::string& username, const std::string& user_password);

    bool operator==(const std::shared_ptr<User> User);
    User &operator=(const std::shared_ptr<User> user);

    void Add_Account(const std::string& name, const char* asset_type, const double& initial_asset_amount, const int &account_number);
    void Delete_Account(std::string& accountID_toDelete);
    std::shared_ptr<Account> Find_Account_By_ID(const std::string &accountID);

    const std::vector<std::shared_ptr<Account>>& Get_Accounts() const { return accounts; }
    const std::string Get_Username() const { return username; }

    void Set_Username(std::string& new_name) { username = new_name; }
    void Set_Password(std::string& new_password) { password = new_password; }

    friend class System;
};
#endif
