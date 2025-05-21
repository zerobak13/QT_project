#pragma once
#include "Stone.h"

#include <QWidget>
#include <QVector>
#include <QPoint>



class BoardWidget : public QWidget
{
    Q_OBJECT

public:
    BoardWidget(int size, QVector<QVector<Stone>>& board, QVector<QPoint>& validMoves, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
private:
    int boardSize;
    QVector<QVector<Stone>>& board;
    QVector<QPoint>& validMoves;
signals:
    void cellClicked(int x, int y);  // GameWindow로 클릭 정보 전달

};
