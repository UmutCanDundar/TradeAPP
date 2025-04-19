#include "server.h"
#include "config_s.h"

#include <QDebug>
#include <QSqlQuery>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>
#include <QThread>
#include <QSqlRecord>

Server::Server(QObject *parent) : QObject(parent), server(new QTcpServer(this)), server_socket(new QTcpSocket(this)), database(new Database(this))
{
    connect(server, &QTcpServer::newConnection, this, &Server::handleClientConnection);
    qDebug() << "serverconst:" << QThread::currentThread();
}

Server::~Server()
{
    qDebug() << "serverdest";
}

void Server::Start_Server()
{
    qDebug() << "serverthread.startserver" << QThread::currentThread();
    QJsonObject json_obj = Config_S::Instance()->Get_Config();

    if(!(server->listen(QHostAddress::Any, json_obj["svport"].toInt())))
        qDebug() << "server listening" << server->errorString();
    else
        qDebug() << "server error";
}

void Server::Stop_Server()
{
    qDebug() << "serverthread.stopserver" << QThread::currentThread();
    qDebug() << "StopS-serversocketstate:" << server_socket->state();
    server->close();
    qDebug() <<  "Server closed";
}

void Server::handleClientConnection()
{
    qDebug() << "handleclientconnection:" <<QThread::currentThread();
    server_socket = server->nextPendingConnection();
    qDebug() << "handleCC-serversocketstate:" << server_socket->state();
    connect(server_socket, &QTcpSocket::readyRead, this, &Server::readClientData);
}

void Server::readClientData()
{
    QByteArray data_values(server_socket->readAll());
    QJsonDocument json_doc1 = QJsonDocument::fromJson(data_values);
    QJsonObject json_obj1 = json_doc1.object();
    QString obj_key = json_obj1.keys().at(0);
    qDebug() << "readclientdata-request:" << obj_key;
    Database::Query query_type = static_cast<Database::Query>(obj_key.toInt());
    QVariantList values = json_obj1[obj_key].toVariant().toList();

    QSqlQuery query;
    switch (query_type) {
    case Database::Query::tablescreate:
        query = database->Execute_Query<Database::Query::tablescreate>(values);
        break;
    case Database::Query::userinsert:
        query = database->Execute_Query<Database::Query::userinsert>(values);
        break;
    case Database::Query::userdelete:
        query = database->Execute_Query<Database::Query::userdelete>(values);
        break;
    case Database::Query::accountadd:
        query = database->Execute_Query<Database::Query::accountadd>(values);
        break;
    case Database::Query::accountdelete:
        query = database->Execute_Query<Database::Query::accountdelete>(values);
        break;
    case Database::Query::accountnumber:
        query = database->Execute_Query<Database::Query::accountnumber>(values);
        break;
    case Database::Query::getaccounts:
        query = database->Execute_Query<Database::Query::getaccounts>(values);
        break;
    default:
        qDebug() << "Unknown query type.";
        break;
    }

    qDebug() << "record count:" << query.record().count();

    QJsonArray json_array;

    while(query.next()) {
        QJsonObject json_obj2;
        for (int i = 0; i < query.record().count();i++) {
            json_obj2[query.record().fieldName(i)] = QJsonValue::fromVariant(query.record().value(i));
            qDebug() << "fieldname" << i << query.record().fieldName(i) << query.record().value(i);
        }
        json_array.append(json_obj2);
    }

    QJsonDocument json_doc2(json_array);
    data_values.clear();
    data_values = json_doc2.toJson();
    qDebug() << "server data value:" << data_values;

    server_socket->write(data_values);
}


