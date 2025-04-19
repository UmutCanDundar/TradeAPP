#include "config_s.h"
#include <QFile>
#include <QJsonDocument>
#include <QByteArray>

Config_S::Config_S()
{
    QFile file(":/config/config/config_DB.json");
    file.open(QIODevice::ReadOnly);
    QByteArray data_DB = file.readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(data_DB);
    json_obj_config = json_doc.object();
    file.close();
}

Config_S* Config_S::Instance()
{
   static Config_S ConfigDB;
   return &ConfigDB;
}

QJsonObject Config_S::Get_Config() const
{
    return json_obj_config;
}
