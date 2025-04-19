#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
#include <QObject>
#include <QByteArray>
#include <QSemaphore>

class Client : public QObject {
    Q_OBJECT

private:
    QTcpSocket *client_socket;
    QByteArray server_response;

public:
    Client(QObject *parent = nullptr);
    ~Client();

    void Send_Request(const QString &query_type, const QVariantList &values = {});

    QByteArray Get_Response() { return server_response; }

signals:
    void Ready();

private slots:
    void handleServerResponse();

};
#endif
