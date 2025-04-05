#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QSslSocket>
#include <QCloseEvent>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void closeEvent(QCloseEvent *event) override;

private Q_SLOTS:
    void socketConnected();
    void socketDisconnected();
    void socketReadyRead();
    void sslErrors(const QList<QSslError> &errors);
    void on_btnConnect_clicked();
    void on_btnDisconnect_clicked();
    void on_btnSend_clicked();
    void on_btnClear_clicked();

private:
    Ui::Widget *ui;
    QTcpSocket* m_tcpSocket;
    QSslSocket* m_sslSocket;
    QAbstractSocket* m_currentSocket;
};

#endif
