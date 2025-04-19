#ifndef CONFIG_S_H
#define CONFIG_S_H

#include <QJsonObject>

class Config_S {

private:
    QJsonObject json_obj_config;

    Config_S();
    Config_S(Config_S &) = delete;
    Config_S &operator=(Config_S &) = delete;
    ~Config_S() = default;

public:
    static Config_S* Instance();
    QJsonObject Get_Config() const;
};
#endif
