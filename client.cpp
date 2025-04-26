#include "client.h"

#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QThread>
#include <QSemaphore>


Client::Client(QObject *parent) : QObject(parent), client_socket(new QTcpSocket(this))
{
    connect(client_socket, &QTcpSocket::readyRead, this, &Client::handleServerResponse);
}

Client::~Client()
{
    client_socket->disconnectFromHost();
    qDebug() << client_socket->state();
    qDebug() << "clientdest";
}

void Client::Send_Request(const QString &query_type, const QVariantList &values)
{
    qDebug() << "send request:" << client_socket->state() <<  query_type;
    if(!(client_socket->state() == QAbstractSocket::ConnectedState))
        client_socket->connectToHost("127.0.0.1", 3307);
qDebug() << "bura1";
    if (client_socket->waitForConnected()) {
        qDebug() << "bura2";
        QJsonArray json_array;
        for (const auto &value : values)
            json_array.append(QJsonValue::fromVariant(value));
        QJsonObject json_obj1;
        json_obj1[query_type] = json_array;
        QJsonDocument json_doc1(json_obj1);
        QByteArray data_values = json_doc1.toJson();
        client_socket->write(data_values);
        qDebug() << "bura3";
    } else {
        qDebug() << "Failed to connect to server:" << client_socket->errorString();
    }
}

void Client::handleServerResponse()
{
    qDebug() << "handleserverresponse";
    server_response.clear();
    server_response = client_socket->readAll();
    emit Ready();
}
