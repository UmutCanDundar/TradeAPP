#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "system.h"
#include "networkmanager.h"
#include "client.h"

#include <QPushButton>
#include <QMainWindow>
#include <QStack>
#include <QIntValidator>
#include <QObject>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    QThread *clientthread;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void Setup_Button(QPushButton *button);
    void Setup_Text_Fields();
    void Setup_DateTime();
    void Setup_Currency_Table();
    void Setup_Connections();

private:
    static System system;
    std::shared_ptr<User> current_user = nullptr;
    Ui::MainWindow *ui;
    QStack<int> page_history;
    NetworkManager* network_manager;
    QIntValidator* validator;

    Client* client;
    bool combo_box_populated = false;

signals:
    void AccountAdded();

private slots:
    void on_phHomeButtonclicked();
    void on_phTransferButtonclicked();
    void onSignupButtonclicked();
    void onLoginButtonclicked();
    void onPreviousButtonclicked();
    void onCreateButtonclicked();
    void onResetButtonclicked();
    void onWelcomeButtonclicked();
    void onLogoutButtonclicked();
    void onAddAccountButtonclicked();
    void onDeleteAccountButtonclicked();
    void onEditAccNameButtonclicked();
};
#endif
