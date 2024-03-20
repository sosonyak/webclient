#include "widget.h"
#include "ui_widget.h"
#include <iostream>

widget::widget(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::widget)
{
    ui->setupUi(this);

    //Socket_.connected(); // signal func. event func.
    QObject::connect(&Socket_, &QAbstractSocket::connected, this, &widget::doConnected);
    QObject::connect(&Socket_, &QAbstractSocket::disconnected, this, &widget::doDisconnected);
    QObject::connect(&Socket_, &QAbstractSocket::readyRead, this, &widget::doReadyRead);

    cb_setting();
    buttonEnabled();
}

widget::~widget()
{
    delete ui;
}

void widget::cb_setting(){
    QStringList socket_lst=(QStringList()<<"TCP"<<"SSL"<<"UDP");
    ui->cbSocket->addItems(socket_lst);
}

void widget::buttonEnabled(){
    // std::cout << Socket_.state() << ", " << Socket_.ConnectedState << ", " << Socket_.UnconnectedState << ", " <<
    //     ui->pteMessage->toPlainText().size() << ", " << ui->pteMessage->toPlainText().length() << std::endl;
    ui->pbConnect->setEnabled(Socket_.state()!=Socket_.ConnectedState);   // when disconnected
    ui->pbDisconnect->setEnabled(Socket_.state()!=Socket_.UnconnectedState); // when connected
    ui->pbSend->setEnabled(Socket_.state()!=Socket_.UnconnectedState);       // when connected
    ui->pbClear->setEnabled(ui->pteMessage->toPlainText().size());          // when plainText filled
}

void widget::doConnected(){
    ui->pteMessage->insertPlainText("Connected\n");
    buttonEnabled();
}

void widget::doDisconnected(){
    ui->pteMessage->insertPlainText("Disconnected\n");
    buttonEnabled();
}

void widget::doReadyRead(){
    ui->pteMessage->insertPlainText(Socket_.readAll());
    buttonEnabled();
}

void widget::on_pbConnect_clicked()
{
    Socket_.connectToHost(ui->leHost->text(), ui->lePort->text().toUShort());                // port 80
    // Socket_.connectToHostEncrypted(ui->leHost->text(), ui->lePort->text().toUShort());       // port 443
    // Socket_.connectToHost(ui->leHost->text(), ui->lePort->text().toUShort());                // port 80
    buttonEnabled();
}

void widget::on_pbDisconnect_clicked()
{
    Socket_.close();
}

void widget::on_pbSend_clicked()
{
    Socket_.write(ui->pteSend->toPlainText().toUtf8());
}


void widget::on_pbClear_clicked()
{
    ui->pteMessage->clear();
    buttonEnabled();
}

void widget::on_cbSocket_currentTextChanged(const QString &arg1)
{
    socket_name = ui->cbSocket->currentText().toUtf8();
}

