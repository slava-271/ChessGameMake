#include "mainwindow.h"

#include <QVBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QNetworkDatagram>

constexpr int TCP_PORT = 45455;
constexpr int UDP_PORT = 45454;
constexpr int BROADCAST_INTERVAL_MS = 2000;

void MainWindow::sendBroadcastRepeat() {
    QByteArray datagram = "CHESS_INVITE";
    udpSocket->writeDatagram(datagram, QHostAddress::Broadcast, UDP_PORT);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    udpSocket(new QUdpSocket(this)),
    tcpServer(new QTcpServer(this)),
    tcpSocket(nullptr)
{
    // Создаём кнопки
    inviteButton = new QPushButton("Пригласить", this);
    listenButton = new QPushButton("Принять приглашение", this);

    // Создаём layout и виджет-контейнер
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->addWidget(inviteButton);
    layout->addWidget(listenButton);
    setCentralWidget(centralWidget);

    // Подключаем сигналы
    connect(inviteButton, &QPushButton::clicked, this, &MainWindow::sendBroadcast);
    connect(listenButton, &QPushButton::clicked, this, &MainWindow::listenForBroadcast);
    connect(tcpServer, &QTcpServer::newConnection, this, &MainWindow::acceptConnection);

    broadcastTimer = new QTimer(this);
    broadcastTimer->setInterval(BROADCAST_INTERVAL_MS);
    connect(broadcastTimer, &QTimer::timeout, this, &MainWindow::sendBroadcastRepeat);

    // Сразу запускаем TCP сервер
    if (!tcpServer->listen(QHostAddress::AnyIPv4, TCP_PORT)) {
        qDebug() << "TCP сервер не запущен:" << tcpServer->errorString();
    }
}

MainWindow::~MainWindow() = default;

void MainWindow::sendBroadcast() {
    // Создаём диалог
    QDialog dialog(this);
    dialog.setWindowTitle("Отправить приглашение");

    // Виджеты ввода
    QLineEdit* nameEdit = new QLineEdit(&dialog);
    QComboBox* colorCombo = new QComboBox(&dialog);
    colorCombo->addItem("Белые");
    colorCombo->addItem("Чёрные");

    QPushButton* sendButton = new QPushButton("Отправить", &dialog);
    QPushButton* cancelButton = new QPushButton("Отмена", &dialog);

    // Разметка
    QFormLayout* formLayout = new QFormLayout();
    formLayout->addRow("Ваше имя:", nameEdit);
    formLayout->addRow("Цвет фигур:", colorCombo);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(sendButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    layout->addLayout(formLayout);
    layout->addLayout(buttonLayout);

    // Обработчик кнопок
    QObject::connect(sendButton, &QPushButton::clicked, [&]() {
        QString name = nameEdit->text().trimmed();
        QString color = colorCombo->currentText();
        if (name.isEmpty()) {
            QMessageBox::warning(&dialog, "Ошибка", "Пожалуйста, введите имя");
            return;
        }

        if (color == "Белые") {
            myCol = WHITE;
            enCol = BLACK;
        }
        else {
            myCol = BLACK;
            enCol = WHITE;
        }

        // Сформировать и отправить сообщение
        QByteArray datagram = QString("CHESS_INVITE;%1;%2").arg(name, color).toUtf8();
        udpSocket->writeDatagram(datagram, QHostAddress::Broadcast, UDP_PORT);
        QMessageBox::information(this, "Приглашение", "Приглашение отправлено");

        dialog.accept();  // Закрыть диалог
    });

    QObject::connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    dialog.exec();  // Показать модальное окно
}

void MainWindow::listenForBroadcast() {
    if (!udpSocket->bind(QHostAddress::AnyIPv4, UDP_PORT, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось привязать UDP сокет");
        return;
    }
    connect(udpSocket, &QUdpSocket::readyRead, this, &MainWindow::processPendingDatagrams);
    QMessageBox::information(this, "Слушаем", "Ожидаем приглашение...");
    processPendingDatagrams();
}

void MainWindow::processPendingDatagrams() {
    while (udpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = udpSocket->receiveDatagram();
        QByteArray data = datagram.data();

        if (data.startsWith("CHESS_INVITE")) {
            QList<QByteArray> parts = data.split(';');
            if (parts.size() < 3)
                continue;  // Неверный формат

            QString playerName = QString::fromUtf8(parts[1]);
            QString color = QString::fromUtf8(parts[2]);
            if (color == "Белые") {
                color = "Вы играете за чёрных";
                myCol = BLACK;
                enCol = WHITE;
            }
            else {
                color =  "Вы играете за белых";
                myCol = WHITE;
                enCol = BLACK;
            }
            QHostAddress sender = datagram.senderAddress();

            int response = QMessageBox::question(this, "Приглашение",
                                                 QString("Приглашение от %1 (%2). IP: %3\nПодключиться?")
                                                     .arg(playerName, color, sender.toString()));
            if (response == QMessageBox::Yes) {
                connectToHost(sender);
            }
        }
    }
}

void MainWindow::connectToHost(QHostAddress address) {
    tcpSocket = new QTcpSocket(this);
    tcpSocket->connectToHost(address, TCP_PORT);

    if (!tcpSocket->waitForConnected(800000)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось подключиться по TCP");
        delete tcpSocket;
        tcpSocket = nullptr;
        return;
    }

    QMessageBox::information(this, "Соединение", "Соединение установлено (клиент)");

    emit connectionEstablished();
}

void MainWindow::acceptConnection() {
    tcpSocket = tcpServer->nextPendingConnection();
    QMessageBox::information(this, "Соединение", "Клиент подключился (сервер)");
    emit connectionEstablished();
}