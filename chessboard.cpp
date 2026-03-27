// chessboard.cpp
#include "chessboard.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QCoreApplication>
#include <QCursor>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

bool avalaible = 0;


void ChessBoard::PlaceInto(ChessPiece* piece) {
    pieceMap[ (1ull << (piece->m_row * 8)) << piece->m_col] = piece;
}

ChessPiece::ChessPiece(QWidget *parent, const QPixmap &pixmap, char type, int row, int col)
    : QWidget(parent), m_pixmap(pixmap), m_type(type), m_row(row), m_col(col), m_dragging(false)
{
    setAttribute(Qt::WA_TranslucentBackground);
}

void ChessPiece::setPosition(int row, int col)
{
    m_row = row;
    m_col = col;
    ChessBoard* board = qobject_cast<ChessBoard*>(parent());
    if (board) {
        QPoint center = board->getSquareCenter(row, col);
        move(center.x() - width()/2, center.y() - height()/2);
    }
}

void ChessPiece::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawPixmap(rect(), m_pixmap);
}

void ChessPiece::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragStart = event->pos();
        m_dragging = true;
        raise();
        ChessBoard* board = qobject_cast<ChessBoard*>(parent());
        if (board) {
            board->setDraggedPiece(this);
        }
    }
}

void ChessPiece::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging) {
        move(mapToParent(event->pos() - m_dragStart));
    }
}

void ChessPiece::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (m_dragging) {
        m_dragging = false;
        ChessBoard* board = qobject_cast<ChessBoard*>(parent());
        if (board) {
            board->pieceDropped(this);
        }
    }
}

ChessBoard::ChessBoard(QWidget *parent)
    : QWidget(parent), m_squareSize(60), m_draggedPiece(nullptr)
{
    setFixedSize(8 * m_squareSize, 8 * m_squareSize);
    loadPiecesImages();

    m_playerIsWhite = myCol ^ 1;

    avalaible = (myCol == WHITE);
    setupBoard();
}


void ChessBoard::setPlayerColor(bool isWhite)
{
    m_playerIsWhite = isWhite;
    setupBoard();
    update();
}

int ChessBoard::getVisualRow(int logicalRow) const
{
    return m_playerIsWhite ? (7 - logicalRow) : logicalRow;
}

int ChessBoard::getVisualCol(int logicalCol) const
{
    return m_playerIsWhite ? logicalCol : (7 - logicalCol);
}

QPoint ChessBoard::getLogicalPosition(const QPoint& visualPos) const
{
    int visualCol = visualPos.x() / m_squareSize;
    int visualRow = visualPos.y() / m_squareSize;

    int logicalRow = m_playerIsWhite ? (7 - visualRow) : visualRow;
    int logicalCol = m_playerIsWhite ? visualCol : (7 - visualCol);

    return QPoint(logicalCol, logicalRow);
}

QPoint ChessBoard::getSquareCenter(int row, int col) const
{
    int visualRow = getVisualRow(row);
    int visualCol = getVisualCol(col);
    return QPoint(visualCol * m_squareSize + m_squareSize / 2,
                  visualRow * m_squareSize + m_squareSize / 2);
}

void ChessBoard::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            int visualRow = getVisualRow(row);
            int visualCol = getVisualCol(col);
            QColor color = (row + col) % 2 ? QColor(210, 180, 140) : QColor(139, 69, 19);
            painter.fillRect(visualCol * m_squareSize, visualRow * m_squareSize,
                             m_squareSize, m_squareSize, color);
        }
    }
}

void ChessBoard::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    m_squareSize = qMin(width(), height()) / 8;
    for (ChessPiece* piece : pieceMap) {
        piece->resize(m_squareSize, m_squareSize);
        piece->setPosition(piece->getBoardPosition().y(), piece->getBoardPosition().x());
    }
}

void ChessBoard::loadPiecesImages()
{
    QString pieceNames[] = {"king", "queen", "rook", "bishop", "knight", "pawn"};

    for (const QString &color : {"white", "black"}) {
        for (const QString &name : pieceNames) {
            QString key = color + "_" + name;
            QString path = ":/pieces/" + key + ".png";
            m_piecesImages[key] = QPixmap(path);
        }
    }
}

void ChessBoard::setupBoard()
{
    qDeleteAll(pieceMap);
    pieceMap.clear();
    placePieces();
}

void ChessBoard::placePieces()
{
    PlaceInto(new ChessPiece(this, m_piecesImages["white_rook"],   'R', 0, 0));
    PlaceInto(new ChessPiece(this, m_piecesImages["white_knight"], 'N', 0, 1));
    PlaceInto(new ChessPiece(this, m_piecesImages["white_bishop"], 'B', 0, 2));
    PlaceInto(new ChessPiece(this, m_piecesImages["white_queen"],  'Q', 0, 3));
    PlaceInto(new ChessPiece(this, m_piecesImages["white_king"],   'K', 0, 4));
    PlaceInto(new ChessPiece(this, m_piecesImages["white_bishop"], 'B', 0, 5));
    PlaceInto(new ChessPiece(this, m_piecesImages["white_knight"], 'N', 0, 6));
    PlaceInto(new ChessPiece(this, m_piecesImages["white_rook"],   'R', 0, 7));

    for (int col = 0; col < 8; ++col) {
        PlaceInto(new ChessPiece(this, m_piecesImages["white_pawn"], 'P', 1, col));
    }

    // Черные фигуры
    PlaceInto(new ChessPiece(this, m_piecesImages["black_rook"],   'r', 7, 0));
    PlaceInto(new ChessPiece(this, m_piecesImages["black_knight"], 'n', 7, 1));
    PlaceInto(new ChessPiece(this, m_piecesImages["black_bishop"], 'b', 7, 2));
    PlaceInto(new ChessPiece(this, m_piecesImages["black_queen"],  'q', 7, 3));
    PlaceInto(new ChessPiece(this, m_piecesImages["black_king"],   'k', 7, 4));
    PlaceInto(new ChessPiece(this, m_piecesImages["black_bishop"], 'b', 7, 5));
    PlaceInto(new ChessPiece(this, m_piecesImages["black_knight"], 'n', 7, 6));
    PlaceInto(new ChessPiece(this, m_piecesImages["black_rook"],   'r', 7, 7));

    for (int col = 0; col < 8; ++col) {
        PlaceInto(new ChessPiece(this, m_piecesImages["black_pawn"], 'p', 6, col));
    }

    for (ChessPiece* piece : pieceMap) {
        piece->resize(m_squareSize, m_squareSize);
        piece->setPosition(piece->getBoardPosition().y(), piece->getBoardPosition().x());
    }
    GenerateMoves();
}

ChessPiece* ChessBoard::pieceAt(const uint64_t index) const
{
    if (pieceMap.contains(index))
        return pieceMap[index];
    return nullptr;
}

void ChessBoard::setDraggedPiece(ChessPiece *piece)
{
    m_draggedPiece = piece;
}

void ChessBoard::pieceDropped(ChessPiece *piece)
{
    QPoint dropPos = mapFromGlobal(QCursor::pos());
    QPoint logicalPos = getLogicalPosition(dropPos);
    uint64_t index =  (1ull << (piece->m_row * 8)) << piece->m_col;
    int y = logicalPos.y();
    int x = logicalPos.x();
    if (logicalPos.x() >= 0 && logicalPos.x() < 8 &&
        logicalPos.y() >= 0 && logicalPos.y() < 8) {
        uint64_t newIndex = (1ull << (y * 8));
        newIndex <<= x;
        Figures fig = ChessBoards[myCol][index];
        uint64_t moves = myMoves[fig] & newIndex;

        ChessPiece* target = pieceAt(newIndex);
        bool isWhite = isupper(piece->getType());

        if (moves && avalaible) {
        if (target) {
            if (isWhite != isupper(target->getType())) {
                pieceMap.remove(newIndex);
                board[myCol][ChessBoards[enCol][newIndex]] = 0;
                ChessBoards[enCol].erase(newIndex);
                delete target;
            } else {
                piece->setPosition(piece->getBoardPosition().y(), piece->getBoardPosition().x());
                m_draggedPiece = nullptr;
                return;
            }
        }

        if (index == board[myCol][KING]) {
            if (((index << 2) == moves) && rightSwapAval) {
                pieceMap[board[myCol][ROOK2]]->setPosition(y, x - 1);
                PlaceInto(pieceMap[board[myCol][ROOK2]]);
                pieceMap.remove(board[myCol][ROOK2]);
                ChessBoards[myCol].erase(board[myCol][ROOK2]);
                board[myCol][ROOK2] >>= 2;
                ChessBoards[myCol][board[myCol][ROOK2]] = ROOK2;

            }
            else if ( ((index >> 2) == moves) && leftSwapAval && ( (board[myCol][ROOK1] << 1) & ~figs[enCol])) {
                pieceMap[board[myCol][ROOK1]]->setPosition(y, x + 1);
                PlaceInto(pieceMap[board[myCol][ROOK1]]);
                pieceMap.remove(board[myCol][ROOK1]);
                ChessBoards[myCol].erase(board[myCol][ROOK1]);
                board[myCol][ROOK1] <<= 3;
                ChessBoards[myCol][board[myCol][ROOK1]] = ROOK1;
            }
            rightSwapAval = false;
            leftSwapAval = false;
        }
        else if (index == board[myCol][ROOK1]) {
            leftSwapAval = false;
        }
        else if (index == board[myCol][ROOK2]) {
            rightSwapAval = false;
        }
        else if (index & currPawns[myCol]) {
            if (newIndex & ~figs[enCol] & ~(index << 8 | index >> 8 | index << 16 | index >> 16)) {
                if (myCol == WHITE) {                   
                    delete pieceMap[newIndex >> 8];
                    board[enCol][ChessBoards[enCol][newIndex >> 8]] = 0;
                    ChessBoards[enCol].erase(newIndex >> 8);
                    pieceMap.remove(newIndex >> 8);
                } else {
                    delete pieceMap[newIndex << 8];
                    board[enCol][ChessBoards[enCol][newIndex << 8]] = 0;
                    ChessBoards[enCol].erase(newIndex << 8);
                    pieceMap.remove(newIndex << 8);
                }
            }
        }

        board[myCol][fig] = moves;
        ChessBoards[myCol].erase(index);
        ChessBoards[myCol][moves] = fig;      
        piece->setPosition(logicalPos.y(), logicalPos.x());
        pieceMap[moves] = pieceMap[index];
        pieceMap.remove(index);
        avalaible = false;
        emit moveMade(index, newIndex, NONE);
        // ReadyForNextStep();
        // GenerateMoves();
    } else {
        piece->setPosition(piece->getBoardPosition().y(), piece->getBoardPosition().x());
    }
    }
    else {
        piece->setPosition(piece->getBoardPosition().y(), piece->getBoardPosition().x());
    }
    m_draggedPiece = nullptr;
}

void ChessBoard::DeleteElement(const uint64_t newIndex) {
    if (newIndex & figs[myCol]) {
    board[myCol][ChessBoards[myCol][newIndex]] = 0;
    ChessBoards[myCol].erase(newIndex);
    delete pieceMap[newIndex];
    }
}



