#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    m_tcpSocket(new QTcpSocket(this)),
    m_sslSocket(new QSslSocket(this)),
    m_currentSocket(nullptr)
{
    ui->setupUi(this);

    ui->btnDisconnect->setEnabled(false);
    ui->btnSend->setEnabled(false);

    connect(m_tcpSocket, &QTcpSocket::connected, this, &Widget::socketConnected);
    connect(m_tcpSocket, &QTcpSocket::disconnected, this, &Widget::socketDisconnected);
    connect(m_tcpSocket, &QTcpSocket::readyRead, this, &Widget::socketReadyRead);

    connect(m_sslSocket, &QSslSocket::connected, this, &Widget::socketConnected);
    connect(m_sslSocket, &QSslSocket::disconnected, this, &Widget::socketDisconnected);
    connect(m_sslSocket, &QSslSocket::readyRead, this, &Widget::socketReadyRead);
    connect(m_sslSocket, &QSslSocket::sslErrors, this, &Widget::sslErrors);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::closeEvent(QCloseEvent *event)
{
    if (m_tcpSocket->state() == QAbstractSocket::ConnectedState)
        m_tcpSocket->disconnectFromHost();
    if (m_sslSocket->state() == QAbstractSocket::ConnectedState)
        m_sslSocket->disconnectFromHost();
    event->accept();
}

void Widget::socketConnected()
{
    static bool isFirstConnect = true;
    if (isFirstConnect) {
        ui->plainTextEdit->appendPlainText("Connected to server");
        isFirstConnect = false;

        ui->btnConnect->setEnabled(false);
        ui->btnDisconnect->setEnabled(true);
        ui->btnSend->setEnabled(true);
    }
}

void Widget::socketDisconnected()
{
    ui->plainTextEdit->appendPlainText("Disconnected from server");
    ui->btnConnect->setEnabled(true);
    ui->btnDisconnect->setEnabled(false);
    ui->btnSend->setEnabled(false);

    if (m_currentSocket) {
        m_currentSocket->abort();
    }
}

void Widget::socketReadyRead()
{
    QByteArray data;
    if (ui->sslCheckBox->isChecked())
        data = m_sslSocket->readAll();
    else
        data = m_tcpSocket->readAll();
    ui->plainTextEdit->appendPlainText(QString::fromUtf8(data));
}

void Widget::sslErrors(const QList<QSslError> &)
{
    m_sslSocket->ignoreSslErrors();
}

void Widget::on_btnConnect_clicked()
{
    QString host = ui->lineEditHost->text();
    int port = ui->lineEditPort->text().toInt();

    if (m_currentSocket) {
        m_currentSocket->abort();
    }

    if (ui->sslCheckBox->isChecked()) {
        m_currentSocket = m_sslSocket;
        m_sslSocket->setPeerVerifyMode(QSslSocket::VerifyNone);
        port = 80;
        m_sslSocket->connectToHostEncrypted(host, port);
    } else {
        m_currentSocket = m_tcpSocket;
        port = 80;
        m_tcpSocket->connectToHost(host, port);
    }
}

void Widget::on_btnDisconnect_clicked()
{
    if (ui->sslCheckBox->isChecked())
        m_sslSocket->disconnectFromHost();
    else
        m_tcpSocket->disconnectFromHost();
}

void Widget::on_btnSend_clicked()
{
    QString requestText = ui->requestText->toPlainText();

    QString request = QString("GET / HTTP/1.1\r\n"
                              "Host: %1\r\n"
                              "Connection: close\r\n"
                              "\r\n").arg(ui->lineEditHost->text());

    QByteArray data = request.toUtf8();

    if (ui->sslCheckBox->isChecked()) {
        if (m_sslSocket->state() == QAbstractSocket::ConnectedState) {
            m_sslSocket->write(data);
        }
    } else {
        if (m_tcpSocket->state() == QAbstractSocket::ConnectedState) {
            m_tcpSocket->write(data);
        }
    }
}

void Widget::on_btnClear_clicked()
{
    ui->plainTextEdit->clear();
}
