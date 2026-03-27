// chessboard.h
#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "bitboard.h"
#include <QWidget>
#include <QPixmap>
#include <QMap>
#include "mainwindow.h"

extern bool avalaible;

class ChessPiece;

class ChessBoard : public QWidget
{
    Q_OBJECT

signals:
    void moveMade(const uint64_t fromIndex,const uint64_t toIndex, const Figures change);
public:
    explicit ChessBoard(QWidget *parent = nullptr);
    void setPlayerColor(bool isWhite); // true - белые, false - черные

    QPoint getSquareCenter(int row, int col) const;
    ChessPiece* pieceAt(const uint64_t index) const;
    void setDraggedPiece(ChessPiece *piece);
    void pieceDropped(ChessPiece *piece);
    QHash<quint64, ChessPiece*> pieceMap;
    void PlaceInto(ChessPiece* piece);
    void DeleteElement(const uint64_t newIndex);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void loadPiecesImages();
    void setupBoard();
    void placePieces();
    int getVisualRow(int logicalRow) const;
    int getVisualCol(int logicalCol) const;
    QPoint getLogicalPosition(const QPoint& visualPos) const;

    int m_squareSize;
    ChessPiece* m_draggedPiece;
    QMap<QString, QPixmap> m_piecesImages;
    void DeleteElement();
    bool m_playerIsWhite;
};

class ChessPiece : public QWidget
{
    Q_OBJECT
public:
    ChessPiece(QWidget *parent, const QPixmap &pixmap, char type, int row, int col);

    void setPosition(int row, int col);
    QPoint getBoardPosition() const { return QPoint(m_col, m_row); }
    char getType() const { return m_type; }
    int m_row;
    int m_col;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QPixmap m_pixmap;
    char m_type;
    bool m_dragging;
    QPoint m_dragStart;
};

#endif // CHESSBOARD_H
