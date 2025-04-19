#include "User.h"

#include <algorithm>

QDebug &operator<< (QDebug& debug, const std::shared_ptr<User>& obj)
{
    debug << obj->username << obj->password;
    for (auto &account : obj->accounts) {
        debug << account;
    }
    return debug;
}

User::User(const std::string& username, const std::string& user_password) : username(username), password(user_password)
{
}

bool User::operator==(const std::shared_ptr<User> User)
{
    return this->username == User->username && this->password == User->password;
}

User &User::operator=(const std::shared_ptr<User> user)
{
    this->username = user->username;
    this->password = user->password;
    std::copy(user->accounts.begin(),user->accounts.end(),this->accounts.begin());
    return *this;
}

void User::Add_Account(const std::string& name, const char* asset_type, const double& initial_asset_amount, const int &account_number)
{
    accounts.push_back(std::make_shared<Account>(name, asset_type, initial_asset_amount, account_number));
}

void User::Delete_Account(std::string& accountID_toDelete)
{
    auto account_it = std::find_if(accounts.begin(), accounts.end(), [&accountID_toDelete] (const std::shared_ptr<Account> account) {

        return accountID_toDelete == account->Get_ID();
    });

    accounts.erase(account_it);
}

std::shared_ptr<Account> User::Find_Account_By_ID(const std::string &accountID)
{
    auto account_it = std::find_if(accounts.begin(), accounts.end(), [&accountID] (const std::shared_ptr<Account> &account) {

        return account->Get_ID() == accountID;
    });

    return *account_it;
}
