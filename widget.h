#ifndef WIDGET_H
#define WIDGET_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QSslSocket>
#include <QUdpSocket>

QT_BEGIN_NAMESPACE
namespace Ui {
class widget;
}
QT_END_NAMESPACE

class widget : public QMainWindow
{
    Q_OBJECT

public:
    widget(QWidget *parent = nullptr);
    ~widget();

    QString socket_name = "TCP";
    QTcpSocket Socket_;
    // QSslSocket Socket_;
    // QUdpSocket Socket_;

    void cb_setting();
    void buttonEnabled();

public slots:
    void doConnected();
    void doDisconnected();
    void doReadyRead();

private slots:
    void on_pbConnect_clicked();

    void on_pbDisconnect_clicked();

    void on_pbSend_clicked();

    void on_pbClear_clicked();

    void on_cbSocket_currentTextChanged(const QString &arg1);

private:
    Ui::widget *ui;
};
#endif // WIDGET_H