#include "BoardWidget.h"
#include <QPainter>
#include <QMouseEvent>



BoardWidget::BoardWidget(int size, QVector<QVector<Stone>>& boardRef, QVector<QPoint>& validRef, QWidget* parent)
    : QWidget(parent), boardSize(size), board(boardRef), validMoves(validRef)
{
    setFixedSize(600, 600);
}

void BoardWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    int cellSize = width() / boardSize;

    painter.fillRect(rect(), QColor(240, 240, 240));
    QPen pen(Qt::black, 2);
    painter.setPen(pen);

    // 격자
    for (int i = 0; i <= boardSize; ++i) {
        painter.drawLine(0, i * cellSize, boardSize * cellSize, i * cellSize);
        painter.drawLine(i * cellSize, 0, i * cellSize, boardSize * cellSize);
    }

    // 돌 그리기
    for (int y = 0; y < boardSize; ++y) {
        for (int x = 0; x < boardSize; ++x) {
            if (board[y][x] == Stone::None) continue;

            if (board[y][x] == Stone::Block) {
                // ✅ 셀 전체를 채우는 사각형
                QRect fullCell(x * cellSize + 1, y * cellSize + 1, cellSize - 2, cellSize - 2);
                painter.setBrush(Qt::darkBlue);
                painter.setPen(Qt::black);
                painter.drawRect(fullCell);
            }
            else {
                // ✅ 일반 돌용 작게 그리는 원
                QRect cell(x * cellSize + 5, y * cellSize + 5, cellSize - 10, cellSize - 10);
                if (board[y][x] == Stone::Black) {
                    painter.setBrush(Qt::black);
                    painter.setPen(Qt::black);
                }
                else if (board[y][x] == Stone::White) {
                    painter.setBrush(Qt::white);
                    painter.setPen(Qt::black);
                }

                painter.drawEllipse(cell);
            }
        }
    }


    // 착수 위치 표시 (밝은 녹색 사각형)
    painter.setBrush(QColor(144, 238, 144));
    painter.setPen(Qt::NoPen);
    for (const QPoint& p : validMoves) {
        painter.fillRect(p.x() * cellSize + 2, p.y() * cellSize + 2, cellSize - 4, cellSize - 4, QColor(144, 238, 144));
    }
}


void BoardWidget::mousePressEvent(QMouseEvent* event)
{
    int cellSize = width() / boardSize;
    int x = event->pos().x() / cellSize;
    int y = event->pos().y() / cellSize;

    if (x < 0 || x >= boardSize || y < 0 || y >= boardSize)
        return;

    QPoint clicked(x, y);

    if (!validMoves.contains(clicked))
        return;
    emit cellClicked(x, y);

    // 🛑 여기서는 board 에 직접 접근 못할 수 있음
    // → signal/slot 방식으로 GameWindow에게 알리는 게 더 좋음
}
