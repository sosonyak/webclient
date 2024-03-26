#include "widget.h"
#include "ui_widget.h"
#include <iostream>

#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>

widget::widget(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::widget)
{
    ui->setupUi(this);

    cb_setting();
    loadWidgetStateFromJson("widget_state.json");
    socket_name = ui->cbSocket->currentText().toUtf8();
    Socket_ = createSocket(socket_name);
    buttonEnabled();

    QObject::connect(Socket_, &QAbstractSocket::connected, this, &widget::doConnected);
    QObject::connect(Socket_, &QAbstractSocket::disconnected, this, &widget::doDisconnected);
    QObject::connect(Socket_, &QAbstractSocket::readyRead, this, &widget::doReadyRead);
}

widget::~widget()
{
    saveWidgetStateToJson("widget_state.json");
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
    ui->pteMessage->insertPlainText("Connected\n"+Socket_->peerAddress().toString()+"\n");
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
    Socket_ = createSocket(socket_name);

    // to reconnect to socket
    QObject::connect(Socket_, &QAbstractSocket::connected, this, &widget::doConnected);
    QObject::connect(Socket_, &QAbstractSocket::disconnected, this, &widget::doDisconnected);
    QObject::connect(Socket_, &QAbstractSocket::readyRead, this, &widget::doReadyRead);

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

void widget::saveWidgetStateToJson(const QString &fileName) {
    QJsonObject widgetState;
    widgetState["socketName"] = socket_name;
    widgetState["host"] = ui->leHost->text();
    widgetState["port"] = ui->lePort->text();
    widgetState["messageToSend"] = ui->pteSend->toPlainText();

    // Get widget geometry and save position and size
    QJsonObject geometry;
    geometry["x"] = this->geometry().x();
    geometry["y"] = this->geometry().y();
    geometry["width"] = this->geometry().width();
    geometry["height"] = this->geometry().height();
    widgetState["geometry"] = geometry;

    QJsonDocument saveDoc(widgetState);
    QFile saveFile(fileName);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open save file.");
        return;
    }
    saveFile.write(saveDoc.toJson());
}

void widget::loadWidgetStateFromJson(const QString &fileName) {
    QFile loadFile(fileName);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open load file.");
        return;
    }

    QByteArray saveData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    QJsonObject widgetState = loadDoc.object();

    socket_name = widgetState["socketName"].toString();
    ui->leHost->setText(widgetState["host"].toString());
    ui->lePort->setText(widgetState["port"].toString());
    ui->pteSend->setPlainText(widgetState["messageToSend"].toString());

    int socketIndex = ui->cbSocket->findText(socket_name);
    if (socketIndex != -1) {
        ui->cbSocket->setCurrentIndex(socketIndex);
    }

    // Restore widget position and size
    QJsonObject geometry = widgetState["geometry"].toObject();
    int x = geometry["x"].toInt();
    int y = geometry["y"].toInt();
    int width = geometry["width"].toInt();
    int height = geometry["height"].toInt();
    this->setGeometry(x, y, width, height);
}
