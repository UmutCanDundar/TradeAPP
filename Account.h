#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "Asset.h"

#include <cstdint>
#include <string>
#include <iomanip>
#include <sstream>

class Account
{
    friend QDebug &operator<< (QDebug& debug, const std::shared_ptr<Account>& obj);

private:
    std::string name;
    std::string ID;
    Asset asset;

public:
    Account(const std::string& name, const char* asset_type, const double& initial_asset_amount, uint32_t account_number);
    bool operator==(const Account& account) const;

    const std::string Get_Account_Name() const { return name; }
    const std::string Get_ID() const { return ID; }

    void Set_Account_Name(std::string&& new_name) { name = new_name; }
};
#endif


