#include "networkmanager.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

NetworkManager::NetworkManager(QObject *parent)
    : QNetworkAccessManager(parent)
{
    connect(this, &QNetworkAccessManager::finished,
            this, &NetworkManager::onReplyFinished);

    connect(&timer, &QTimer::timeout,
            this, &NetworkManager::Fetch_Exchange_Rates);
}

void NetworkManager::Fetch_Exchange_Rates()
{
    QUrl url("https://api.exchangerate-api.com/v4/latest/TRY");
    QNetworkRequest request(url);
    if(get(request)->error() != QNetworkReply::NoError);/* {
         url("https://api.exchangerate.host/latest?base=TRY");
         get request(url);
    }*/
}

void NetworkManager::onReplyFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response_data = reply->readAll();
        Parse_Exchange_Rates(response_data);
        emit exchangeRatesUpdated();
    } else {
        qWarning() << "Request failed:" << reply->errorString();
    }

    reply->deleteLater();
}

void NetworkManager::Parse_Exchange_Rates(const QByteArray &data)
{
    QJsonDocument json_doc = QJsonDocument::fromJson(data);
    QJsonObject json_obj = json_doc.object();
    QJsonObject json_obj_rates = json_obj["rates"].toObject();
    Map_exchange_rates.clear();

    for (const QString &key : json_obj_rates.keys()) {
        if(key == "USD" || key == "GBP" || key == "TRY" || key == "EUR")
            Map_exchange_rates.insert(key, 1 / json_obj_rates[key].toDouble());
        }
}

const QMap<QString,double> NetworkManager::Get_Exchange_Rates() const
{
    return Map_exchange_rates;
}

void NetworkManager::Start_Fetching(int interval)
{
    timer.start(interval);
}

