#ifndef SERVER_H
#define SERVER_H

#include "database.h"

#include <QTcpServer>
#include <QTcpSocket>

class Server : public QObject {
    Q_OBJECT

private:
    QTcpServer *server;
    QTcpSocket *server_socket;
    Database *database;

public:
    Server(QObject *parent = nullptr);
    ~Server();

public slots:
    void Start_Server();
    void Stop_Server();

private slots:
    void handleClientConnection();
    void readClientData();
};
#endif


