#include "widget.h"
#include "ui_widget.h"

widget::widget(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::widget)
{
    ui->setupUi(this);

    QObject::connect(Socket_, &QAbstractSocket::connected, this, &widget::doConnected);
    QObject::connect(Socket_, &QAbstractSocket::disconnected, this, &widget::doDisconnected);
    QObject::connect(Socket_, &QAbstractSocket::readyRead, this, &widget::doReadyRead);

    cb_setting();
    socket_name = ui->cbSocket->currentText().toUtf8();
    Socket_ = createSocket(socket_name);
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
    ui->pbConnect->setEnabled(Socket_->state()!=Socket_->ConnectedState);   // when disconnected
    ui->pbDisconnect->setEnabled(Socket_->state()!=Socket_->UnconnectedState); // when connected
    ui->pbSend->setEnabled(Socket_->state()!=Socket_->UnconnectedState);       // when connected
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
    ui->pteMessage->insertPlainText(Socket_->readAll());
    buttonEnabled();
}

QAbstractSocket* widget::createSocket(const QString& name){
    if(name=="TCP") return new QTcpSocket();
    else if(name=="UDP") return new QUdpSocket();
    if(name=="SSL") return new QSslSocket();
}

void widget::on_pbConnect_clicked()
{
    if (Socket_) delete Socket_;

    if (socket_name == "TCP"){
        dynamic_cast<QTcpSocket*>(Socket_)->connectToHost(ui->leHost->text(), ui->lePort->text().toUShort());
    }
    else if (socket_name == "UDP"){
        dynamic_cast<QUdpSocket*>(Socket_)->connectToHost(ui->leHost->text(), ui->lePort->text().toUShort());
    }
    else if (socket_name == "SSL"){
        dynamic_cast<QSslSocket*>(Socket_)->connectToHostEncrypted(ui->leHost->text(), ui->lePort->text().toUShort());
    }
}

void widget::on_pbDisconnect_clicked()
{
    Socket_->close();
}

void widget::on_pbSend_clicked()
{
    Socket_->write(ui->pteSend->toPlainText().toUtf8());
}


void widget::on_pbClear_clicked()
{
    ui->pteMessage->clear();
    buttonEnabled();
}

void widget::on_cbSocket_currentTextChanged(const QString &arg1)
{
    socket_name = ui->cbSocket->currentText().toUtf8();
    ui->lePort->clear();

    if (socket_name!="SSL") ui->lePort->insert((QString)"80");
    else ui->lePort->insert((QString)"443");
}

