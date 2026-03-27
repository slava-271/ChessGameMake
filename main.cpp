// #include <QApplication>
// #include "chessboard.h"

// int main(int argc, char *argv[])
// {
//     QApplication a(argc, argv);

//     ChessBoard board;
//     board.show();

//     return a.exec();
// }

#include <QApplication>
#include "mainwindow.h"
#include "chessboard.h"
#include <bit>

ChessBoard* gameBoard;




int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MainWindow*  window = new MainWindow();
    QObject::connect(window, &MainWindow::connectionEstablished, [&]() {
        gameBoard = new ChessBoard;
        gameBoard->show();

        QObject::connect(gameBoard, &ChessBoard::moveMade, [&](const uint64_t index, const uint64_t newIndex, const Figures fig) {
            QByteArray data;
            data.append(reinterpret_cast<const char*>(&index), sizeof(newIndex));
            data.append(reinterpret_cast<const char*>(&newIndex), sizeof(newIndex));
            data.append(static_cast<char>(fig));

            if (window->tcpSocket->state() == QAbstractSocket::ConnectedState) {
                window->tcpSocket->write(data);
            }
        });


        QObject::connect(window->tcpSocket, &QTcpSocket::readyRead, [&]() {
            QTcpSocket* socket = window->tcpSocket;

                QByteArray data = socket->read(sizeof(uint64_t) * 2 + 1);

                if (data.size() != sizeof(uint64_t) * 2 + 1) return;  // неполный пакет

                uint64_t index, newIndex;
                Figures fig;

                memcpy(&index, data.constData(), sizeof(uint64_t));
                memcpy(&newIndex,   data.constData() + sizeof(uint64_t), sizeof(uint64_t));
                fig = static_cast<Figures>(data[sizeof(uint64_t) * 2]);

                ReadyForNextStep();

                uint64_t y = std::countr_zero(newIndex) / 8;
                uint64_t x = std::countr_zero(newIndex) % 8;

                if (index == board[enCol][KING]) {
                    if (((index << 2) == newIndex)) {
                        gameBoard->pieceMap[board[enCol][ROOK2]]->setPosition(y, x - 1);
                        gameBoard->PlaceInto(gameBoard->pieceMap[board[enCol][ROOK2]]);
                        gameBoard->pieceMap.remove(board[enCol][ROOK2]);
                        ChessBoards[enCol].erase(board[enCol][ROOK2]);
                        board[enCol][ROOK2] >>= 2;
                        ChessBoards[enCol][board[enCol][ROOK2]] = ROOK2;

                    }
                    else if ( ((index >> 2) == newIndex)) {
                        gameBoard->pieceMap[board[enCol][ROOK1]]->setPosition(y, x + 1);
                        gameBoard->PlaceInto(gameBoard->pieceMap[board[enCol][ROOK1]]);
                        gameBoard->pieceMap.remove(board[enCol][ROOK1]);
                        ChessBoards[enCol].erase(board[enCol][ROOK1]);
                        board[enCol][ROOK1] <<= 2;
                        ChessBoards[enCol][board[enCol][ROOK1]] = ROOK1;
                    }
                }
                else if (myFigs[ChessBoards[enCol][index]] <= PAWN8) {
                    if (newIndex & ~figs[myCol] & ~(index << 8 | index >> 8 | index << 16 | index >> 16)) {
                        if (enCol == WHITE) {
                            delete gameBoard->pieceMap[newIndex >> 8];
                            board[myCol][ChessBoards[myCol][newIndex >> 8]] = 0;
                            ChessBoards[myCol].erase(newIndex >> 8);
                            gameBoard->pieceMap.remove(newIndex >> 8);
                        } else {
                            delete gameBoard->pieceMap[newIndex << 8];
                            board[myCol][ChessBoards[myCol][newIndex << 8]] = 0;
                            ChessBoards[myCol].erase(newIndex << 8);
                            gameBoard->pieceMap.remove(newIndex << 8);
                        }
                    }
                }

                gameBoard->DeleteElement(newIndex);

                board[enCol][ChessBoards[enCol][index]] = newIndex;
                ChessBoards[enCol][newIndex] = ChessBoards[enCol][index];
                ChessBoards[enCol].erase(index);
                gameBoard->pieceMap[index]->setPosition(y, x);
                gameBoard->pieceMap[newIndex] = gameBoard->pieceMap[index];
                gameBoard->pieceMap.remove(index);
                ReadyForNextStep();
                GenerateMoves();
                avalaible = true;

        });



    });

    window->show();

    return app.exec();
}
