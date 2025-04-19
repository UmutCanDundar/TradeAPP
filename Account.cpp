#include "Account.h"

QDebug &operator<< (QDebug& debug, const std::shared_ptr<Account>& obj)
{
    debug << "Name: " <<obj->name << "\nID: " <<obj->ID << "\nBalance: " << obj->asset;
    return debug;
}

Account::Account(const std::string& name, const char* asset_type, const double& initial_asset_amount, uint32_t account_number)
    : name(name), asset(asset_type, initial_asset_amount)
{
    account_number++;

    std::ostringstream oss;
    const char* temp = asset.Get_Asset_Type();

    for (size_t index = 0; temp[index] != '\0'; index++)
        oss << std::hex << static_cast<int>(temp[index]);

    oss << std::setw(9) << std::setfill('0') << account_number;
    ID = oss.str();
}

bool Account::operator==(const Account& account) const
{
    return ID == account.ID;
}
