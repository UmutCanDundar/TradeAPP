#ifndef ASSET_H
#define ASSET_H

#include <QDebug>
#include <cstring>

class Asset
{
    friend QDebug &operator<< (QDebug& debug, const Asset &obj);

private:
    char type [21];
    double amount;

public:
    Asset(const char* asset_type,const double& initial_asset_amount = 0.0);

    void Buy(double& amount);
    void Sell(double& amount);

    const char* Get_Asset_Type() const { return type; }
};
#endif
