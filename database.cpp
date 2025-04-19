#include "database.h"
#include "config_s.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QThread>

Database::Database(QObject *parent) : QObject(parent)
{
    qDebug() << "database thread:" << QThread::currentThread();
    db = QSqlDatabase::addDatabase("QMYSQL", "TradeAppDB");
    QJsonObject json_obj = Config_S::Instance()->Get_Config();

    db.setUserName(json_obj["username"].toString());
    db.setPassword(json_obj["password"].toString());
    db.setHostName(json_obj["host"].toString());
    db.setPort(json_obj["dbport"].toInt());
    db.setDatabaseName(json_obj["name"].toString());
    qDebug() << db.open();
    qDebug() << db.lastError().text();
}

Database::~Database()
{
    db.close();
    QSqlDatabase::removeDatabase("TradeAppDB");
    qDebug() << "databasedest";
}

template<Database::Query T>
QSqlQuery Database::Execute_Query(const QVariantList &values)
{
    QSqlQuery sql_query(db);

    Prepare_Query<T>(sql_query);

    if(values.size() > 0) {
        for (const auto &value : values) {
            sql_query.addBindValue(value);
        }
    }

    if (sql_query.exec())
        qDebug() << "Query exec succeed:" << sql_query.lastQuery();
    else
        qDebug() << "Query execution failed:" << sql_query.lastError().text();

    return sql_query;
}


template<>
void Database::Prepare_Query<Database::Query::tablescreate>(QSqlQuery &sql_query)
{
    sql_query.prepare("CREATE TABLE IF NOT EXISTS users ("
                     "user_id INT AUTO_INCREMENT PRIMARY KEY,"
                     "username VARCHAR(50) UNIQUE NOT NULL,"
                     "password VARCHAR(50) NOT NULL,"
                     "email VARCHAR(50),"
                     "created_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
                     "updated_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP"
                     ") ENGINE=InnoDB, CHARSET=utf8mb4" );

    sql_query.exec();

    sql_query.prepare("CREATE TABLE IF NOT EXISTS accounts ("
                     "user_id INT,"
                     "username VARCHAR(50),"
                     "account_id VARCHAR(30) PRIMARY KEY UNIQUE,"
                     "account_name VARCHAR(50),"
                     "balance DECIMAL(14,4) NOT NULL DEFAULT 0.00,"
                     "asset_type VARCHAR(4) NOT NULL DEFAULT 'TRY',"
                     "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
                     "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,"
                     "FOREIGN KEY (user_id)  REFERENCES users(user_id) ON DELETE CASCADE,"
                     "FOREIGN KEY (username) REFERENCES users(username) ON DELETE CASCADE"
                     ") ENGINE=InnoDB, CHARSET=utf8mb4" );
}

template<>
void Database::Prepare_Query<Database::Query::userinsert>(QSqlQuery &sql_query)
{
    sql_query.prepare("INSERT INTO users (username, password, email) VALUES (?,?,?)");
}

template<>
void Database::Prepare_Query<Database::Query::userdelete>(QSqlQuery &sql_query)
{
    sql_query.prepare("DELETE FROM users WHERE username = ?");
}

template<>
void Database::Prepare_Query<Database::Query::accountadd>(QSqlQuery &sql_query)
{
    sql_query.prepare("INSERT IGNORE INTO accounts (user_id, username, account_id, account_name, balance, asset_type)"
                      "VALUES ((SELECT user_id FROM users WHERE username = ?), ?, ?, ?, ?, ?)");
}

template<>
void Database::Prepare_Query<Database::Query::accountdelete>(QSqlQuery &sql_query)
{
    sql_query.prepare("DELETE FROM accounts WHERE account_id = ?");
}

template<>
void Database::Prepare_Query<Database::Query::accountnumber>(QSqlQuery &sql_query)
{
    sql_query.prepare("SELECT RIGHT(account_id , 9) AS account_id9 FROM accounts ORDER BY account_id DESC LIMIT 1");
}

template<>
void Database::Prepare_Query<Database::Query::getaccounts>(QSqlQuery &sql_query)
{
    sql_query.prepare("SELECT * FROM accounts WHERE username = ?");
}

template QSqlQuery Database::Execute_Query<Database::Query::tablescreate>(const QVariantList &);
template QSqlQuery Database::Execute_Query<Database::Query::userinsert>(const QVariantList &);
template QSqlQuery Database::Execute_Query<Database::Query::userdelete>(const QVariantList &);
template QSqlQuery Database::Execute_Query<Database::Query::accountadd>(const QVariantList &);
template QSqlQuery Database::Execute_Query<Database::Query::accountdelete>(const QVariantList &);
template QSqlQuery Database::Execute_Query<Database::Query::accountnumber>(const QVariantList &);
template QSqlQuery Database::Execute_Query<Database::Query::getaccounts>(const QVariantList &);
