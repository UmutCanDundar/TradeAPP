#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "auxiliary.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QDateTime>
#include <QLocale>
#include <QStandardItemModel>
#include <QThread>
#include <QVariantList>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSemaphore>

System MainWindow::system;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow),
    network_manager(new NetworkManager(this)), validator(new QIntValidator(this))
{
    qDebug() << "mainwindow thread" << QThread::currentThread();

    clientthread = new QThread();
    client = nullptr;
    QObject::connect(clientthread, &QThread::started, [this] () {
        client = new Client();
        connect(clientthread, &QThread::finished, client, &QObject::deleteLater);
        client->Send_Request("0");
    });
    clientthread->start();

    ui->setupUi(this);
    QList<QPushButton*> Buttons = findChildren<QPushButton*>();
    for(QPushButton* button : Buttons) {
        Setup_Button(button);
    }
    Setup_Text_Fields();
    Setup_DateTime();
    Setup_Currency_Table();
    Setup_Connections();
}

MainWindow::~MainWindow()
{
    delete ui;
    qDebug() << "mainwindowdest";
}

void MainWindow::on_phHomeButtonclicked()
{
    if(ui->stackedWidget_inner->currentIndex() != 0) {
        ui->stackedWidget_inner->setCurrentIndex(0);
        page_history.push(0);
    }
}

void MainWindow::onPreviousButtonclicked()
{
    if(page_history.size() > 1) {
        page_history.pop();
        ui->stackedWidget_inner->setCurrentIndex(page_history.top());
    }

}

void MainWindow::on_phTransferButtonclicked()
{
    if(ui->stackedWidget_inner->currentIndex() != 1) {
        ui->stackedWidget_inner->setCurrentIndex(1);
        page_history.push(1);
    }
}

void MainWindow::onSignupButtonclicked()
{
    ui->stackedWidget_main->setCurrentIndex(1);
}

void MainWindow::onLoginButtonclicked()
{
    std::string username = ui->LE_Username->text().toStdString();
    std::string password = ui->LE_Password->text().toStdString();
    QSemaphore sema(0);

    if(system.Check_User(username, password)) {
        current_user = *(system.Current_User(username, password));

        QVariantList values;
        values << QString::fromStdString(current_user->Get_Username());
        qDebug() << "values" << values;
        QMetaObject::invokeMethod(client,[this, &values, &sema] {
            client->Send_Request("6", values);

            connect(client, &Client::Ready, [&sema, this]() {
                sema.release();
            });

        }, Qt::QueuedConnection);

        sema.acquire();

        QByteArray data_account = client->Get_Response();
        QJsonDocument json_doc = QJsonDocument::fromJson(data_account);
        QJsonArray json_array = json_doc.array();
        for (const auto &json_array_value : json_array) {
            QJsonObject json_obj = json_array_value.toObject();

            QString account_str;
            QDebug stream(&account_str);
            stream << "Name: " << json_obj["account_name"].toString() << "\nID: " << json_obj["account_id"].toString()
                   << "\nBalance: " << json_obj["balance"].toDouble() << json_obj["asset_type"].toString();

            QListWidgetItem *Item = new QListWidgetItem(account_str, ui->listWidget_Accounts);
            Item->setData(Qt::UserRole, QVariant::fromValue(json_obj["account_id"]));
            Item->setData(Qt::DisplayRole, QVariant::fromValue(account_str));
        }
        ui->stackedWidget_main->setCurrentIndex(2);
        ui->stackedWidget_inner->setCurrentIndex(0);
        page_history.push(0);
        ui->ErrorLabel_1->hide();
        qDebug() << "shared_count:" << system.Current_User(username, password)->use_count();
    } else {
        ui->ErrorLabel_1->show();
    }
}

void MainWindow::onCreateButtonclicked()
{
    std::string new_username = ui->LE_NewUsername->text().toStdString();
    std::string new_password = ui->LE_NewPassword->text().toStdString();
    QSemaphore sema(0);

    if(Is_Only_Characters(new_username)) {
        if(system.Create_User(new_username, new_password)) {

            QVariantList values;
            values << QString::fromStdString(new_username) << QString::fromStdString(new_password) << "";
            QMetaObject::invokeMethod(client,[this, &values, &sema] {
                client->Send_Request("1", values);
                connect(client, &Client::Ready, [&sema, this]() {
                    sema.release();
                });
            }, Qt::QueuedConnection);

            sema.acquire();

            ui->stackedWidget_main->setCurrentIndex(0);
            ui->LE_NewUsername->clear();
            ui->LE_NewPassword->clear();
            ui->ErrorLabel_2->hide();
            qDebug() << "kişi" << system;
        } else {
            ui->ErrorLabel_2->setText("This username is already taken!");
            ui->ErrorLabel_2->show();
        }
    } else {
        ui->ErrorLabel_2->setText("Username must not contain spaces!");
        ui->ErrorLabel_2->show();
    }
}

void MainWindow::onResetButtonclicked()
{
    ui->LE_NewUsername->clear();
    ui->LE_NewPassword->clear();
}

void MainWindow::onWelcomeButtonclicked()
{
    ui->stackedWidget_main->setCurrentIndex(0);
}

void MainWindow::onLogoutButtonclicked()
{
    current_user = nullptr;
    ui->stackedWidget_main->setCurrentIndex(0);
    ui->listWidget_Accounts->clear();
}


void MainWindow::onAddAccountButtonclicked()
{
    QString username = QString::fromStdString(current_user->Get_Username());
    std::string account_name = ui->LE_AccountName->text().toStdString();
    std::string asset_type = ui->AssetTypeComboBox->currentText().toStdString();
    double asset_initial_amount = ui->LE_Amount->text().toDouble();
    uint32_t last_account_number = 0;
    QSemaphore sema(0);

    QMetaObject::invokeMethod(client,[this, &sema] {
        client->Send_Request("5");
        connect(client, &Client::Ready, [&sema, this]() {
            sema.release(); 
        });
    }, Qt::QueuedConnection);

    sema.acquire();

    QByteArray data_account_id = client->Get_Response();
    QJsonDocument json_doc = QJsonDocument::fromJson(data_account_id);
    QJsonArray json_array = json_doc.array();
    QJsonObject json_obj = json_array.at(0).toObject();
    QString last_account_id = json_obj["account_id9"].toString();
    last_account_number = last_account_id.right(9).toInt();


    if(!(account_name.empty() && asset_type.empty() && asset_initial_amount < 0)) {
        current_user->Add_Account(account_name, asset_type.c_str(), asset_initial_amount, last_account_number);
        std::shared_ptr<Account> last_account = current_user->Get_Accounts().back();

        QVariantList values;
        values << username << username
               << QString::fromStdString(last_account->Get_ID()) << QString::fromStdString(account_name)
               << asset_initial_amount << QString::fromStdString(asset_type);
        QMetaObject::invokeMethod(client,[this, values] {
            client->Send_Request("3", values);
        }, Qt::QueuedConnection);

        QString account_str;
        QDebug stream(&account_str);
        stream << last_account;
        QListWidgetItem *Item = new QListWidgetItem(account_str);
        Item->setData(Qt::UserRole, QVariant::fromValue(last_account->Get_ID()));
        Item->setData(Qt::DisplayRole, QVariant::fromValue(account_str));
        ui->listWidget_Accounts->addItem(Item);

        ui->LE_AccountName->clear();
        ui->LE_Amount->clear();
    }
}

void MainWindow::onDeleteAccountButtonclicked()
{
    if(ui->listWidget_Accounts->currentItem() != nullptr) {
        QListWidgetItem *selected_account = ui->listWidget_Accounts->currentItem();

        QMessageBox Confirm;
        Confirm.setWindowTitle("Warning!");
        Confirm.setText("Do you really want to remove this account?");
        Confirm.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

        if(Confirm.exec() == QMessageBox::Yes){
            std::string accountID_toDelete = selected_account->data(Qt::UserRole).value<std::string>();
            current_user->Delete_Account(accountID_toDelete);
            ui->listWidget_Accounts->takeItem(ui->listWidget_Accounts->row(selected_account));
        }
    }
}

void MainWindow::onEditAccNameButtonclicked()
{
    if(ui->listWidget_Accounts->currentItem() != nullptr) {
        QListWidgetItem *selected_account = ui->listWidget_Accounts->currentItem();
        std::string accountID_toEdit = selected_account->data(Qt::UserRole).value<std::string>();
        std::shared_ptr<Account> account_toEdit = current_user->Find_Account_By_ID(accountID_toEdit);

        QInputDialog Edit;
        QString new_account_name = Edit.getText(this,"Edit Account Name","Enter New Name:");

        if(!new_account_name.isEmpty()) {
            account_toEdit->Set_Account_Name(new_account_name.toStdString());

            QString account_str;
            QDebug stream(&account_str);
            stream << account_toEdit;
            selected_account->setData(Qt::DisplayRole, QVariant::fromValue(account_str));
        }
    }
}

void MainWindow::Setup_Button(QPushButton* button)
{
    if(button->objectName().contains("Previous") ||
        button->objectName().contains("Welcome")) {

        button->setStyleSheet(

            "QPushButton { "
            " background-color: #ffffff; "
            "}"
            "QPushButton:hover {"
            " background-color: #acafb0;"
            "} "
            "QPushButton:pressed {"
            " background-color: #757778;"
            "}"
            );

        connect(button,&QPushButton::clicked,
                this ,&MainWindow::onPreviousButtonclicked);
    }

    if(button->objectName().contains("_ph")) {
        button->setStyleSheet(

            "QPushButton { "
            " background-color: #000000; "
            " color: #004275 "
            "}"
            "QPushButton:hover {"
            " background-color: #757778; "
            "} "
            "QPushButton:pressed {"
            " background-color: #757778; "
            "}"
            );
    }
}

void MainWindow::Setup_Text_Fields()
{
    ui->LE_Password->setEchoMode(QLineEdit::Password);
    ui->LE_NewPassword->setEchoMode(QLineEdit::Password);

    validator->setRange(0,10000000);
    ui->LE_Amount->setValidator(validator);

    ui->ErrorLabel_1->hide();
    ui->ErrorLabel_1->setStyleSheet("color: #ff0000;");
    ui->ErrorLabel_2->hide();
    ui->ErrorLabel_2->setStyleSheet("color: #ff0000;");
}

void MainWindow::Setup_DateTime()
{
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout,this, [this]() {
        QDateTime current_date_time = QDateTime::currentDateTime();
        ui->L_Date->setText(current_date_time.toString("dd MMMM yyyy   HH:mm:ss"));
    });
    timer->start(1000);
}

void MainWindow::Setup_Currency_Table()
{
    connect(network_manager, &NetworkManager::exchangeRatesUpdated, this, [this]() {

        QMap<QString, double> Map_exchange_rates = network_manager->Get_Exchange_Rates();

        if(!combo_box_populated) {
            ui->AssetTypeComboBox->clear();
            ui->AssetTypeComboBox->addItems(Map_exchange_rates.keys());
            combo_box_populated = true;
        }

        QStandardItemModel* table_model = new QStandardItemModel(Map_exchange_rates.size(), 1, this);
        table_model->setHorizontalHeaderLabels({"Currency", "TRY"});
        int row = 0;
        for (auto it = Map_exchange_rates.begin(); it != Map_exchange_rates.end(); ++row, ++it){
            table_model->setItem(row, 0, new QStandardItem(it.key()));
            table_model->setItem(row, 1, new QStandardItem(QString::number(it.value(),'f', 5)));

        }

        QHeaderView* table_header =  ui->ExchangeRatesTable->horizontalHeader();
        table_header->setSectionResizeMode(QHeaderView::Stretch);
        table_header->setSectionsClickable(false);
        table_header->setStyleSheet("QHeaderView::section {"
                                    "background-color: #ff0000; "
                                    "color: #ffffff; }");

        table_header->setDefaultSectionSize(table_header->width()/2);
        ui->ExchangeRatesTable->setModel(table_model);

    });

    network_manager->Start_Fetching(1000);
}

void MainWindow::Setup_Connections()
{
    connect(ui->_phHomeButton, &QPushButton::clicked,
            this, &MainWindow::on_phHomeButtonclicked);
    connect(ui->_phTransferButton, &QPushButton::clicked,
            this, &MainWindow::on_phTransferButtonclicked);
    connect(ui->SignupButton, &QPushButton::clicked,
            this, &MainWindow::onSignupButtonclicked);
    connect(ui->CreateButton, &QPushButton::clicked,
            this, &MainWindow::onCreateButtonclicked);
    connect(ui->ResetButton, &QPushButton::clicked,
            this, &MainWindow::onResetButtonclicked);
    connect(ui->LoginButton, &QPushButton::clicked,
            this, &MainWindow::onLoginButtonclicked);
    connect(ui->WelcomeButton, &QPushButton::clicked,
            this, &MainWindow::onWelcomeButtonclicked);
    connect(ui->LogoutButton, &QPushButton::clicked,
            this, &MainWindow::onLogoutButtonclicked);
    connect(ui->AddAccountButton, &QPushButton::clicked,
            this, &MainWindow::onAddAccountButtonclicked);
    connect(ui->DeleteAccountButton, &QPushButton::clicked,
            this, &MainWindow::onDeleteAccountButtonclicked);
    connect(ui->EditAccNameButton, &QPushButton::clicked,
            this, &MainWindow::onEditAccNameButtonclicked);
}

