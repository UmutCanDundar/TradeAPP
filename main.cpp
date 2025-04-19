#include "mainwindow.h"
#include "server.h"

#include <QThread>
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QObject>
#include <QSemaphore>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::addLibraryPath("C:/Qt/6.8.1/mingw_64/plugins/sqldrivers");
    qDebug() << "Plugin Paths: " << QCoreApplication::libraryPaths();
    qDebug() << QSqlDatabase::drivers();

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "TradeAppQt_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    QThread *serverthread = new QThread();
    Server *server = nullptr;
    QSemaphore serverStarted(0);
    QObject::connect(serverthread, &QThread::started, [&server, &serverthread, &serverStarted] () {
        server = new Server();
        server->Start_Server();
        QObject::connect(serverthread, &QThread::finished, server, &Server::Stop_Server);
        QObject::connect(serverthread, &QThread::finished, server, &QObject::deleteLater);
        serverStarted.release();
    });
    serverthread->start();
    serverStarted.acquire();

    MainWindow w;
    QThread *clientthread = w.clientthread;

    QObject::connect(&a, &QApplication::lastWindowClosed, &w, [&clientthread, &serverthread]() {

        qDebug() << "Server thread about to quit";
        clientthread->quit();
        clientthread->wait();
        clientthread->deleteLater();
        qDebug() <<  "buradayız";
        qDebug() << "lastwindowthread:" <<QThread::currentThread();

        serverthread->quit();
        serverthread->wait();
        serverthread->deleteLater();

        qDebug() << "serverthreaddest";
     }, Qt::QueuedConnection);

    w.show();
    return a.exec();
}
