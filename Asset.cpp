#include "Asset.h"

QDebug &operator<< (QDebug& debug, const Asset &obj)
{

    debug << obj.amount << obj.type;
    return debug;
}

Asset::Asset(const char* asset_type,const double& initial_asset_amount)
{
    std::strcpy(type, asset_type);
    amount = initial_asset_amount;
}

void Asset::Buy(double& amount)
{
    this->amount += amount;
}

void Asset::Sell(double& amount)
{
    if (amount <= this->amount)
        this->amount -= amount;
    /* else
         .... */
}
