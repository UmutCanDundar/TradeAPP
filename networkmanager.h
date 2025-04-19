#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QStringList>
#include <QMap>
#include <QTimer>

class NetworkManager : public QNetworkAccessManager {
    Q_OBJECT

public:
    NetworkManager(QObject *parent = nullptr);

    void Fetch_Exchange_Rates();
    const QMap<QString,double> Get_Exchange_Rates() const;
    void Start_Fetching(int interval);

signals:
    void exchangeRatesUpdated();

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    void Parse_Exchange_Rates(const QByteArray &data);

    QMap<QString, double> Map_exchange_rates;
    QTimer timer;
};
#endif


