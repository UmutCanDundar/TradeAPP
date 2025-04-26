#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>

class Database : public QObject {
    Q_OBJECT

private:
   QSqlDatabase db;

public:
    enum class Query : uint8_t {tablescreate, userinsert, userdelete, accountadd, accountdelete, accountnumber, getaccounts, accountedit};
    Q_ENUM (Query)

    Database(QObject *parent = nullptr);
    ~Database();

    template<Query T>
    QSqlQuery Execute_Query(const QVariantList &values);

protected:
    template<Query T>
    void Prepare_Query(QSqlQuery &SqlQuery);

};
#endif
