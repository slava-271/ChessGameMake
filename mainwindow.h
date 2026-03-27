#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>
#include <QTimer>
#include <QLineEdit>
#include <QComboBox>
#include <QFormLayout>
#include "bitboard.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

signals:
    void connectionEstablished();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QTcpSocket *tcpSocket;
    void ReadTcp();

private slots:
    void sendBroadcast();
    void listenForBroadcast();
    void sendBroadcastRepeat();
    void processPendingDatagrams();
    void acceptConnection();
    void connectToHost(QHostAddress address);

private:
    QPushButton *inviteButton;
    QPushButton *listenButton;

    QUdpSocket *udpSocket;
    QTcpServer *tcpServer;


    QTimer *broadcastTimer;

    void stopBroadcasting();
};

#endif // MAINWINDOW_H
