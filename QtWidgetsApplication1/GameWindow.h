#pragma once
#include "Stone.h"

#include <QWidget>
#include <QVector>
#include <QPoint>
#include <QLabel>



class GameWindow : public QWidget
{
    Q_OBJECT

public:
    GameWindow(int boardSize, int obstacleCount, QString firstPlayer, QWidget* parent = nullptr);

    ~GameWindow();

protected:
    
    void updateValidMoves(Stone currentTurn);  
   

private:
    int boardSize;
    int obstacleCount;
    QString firstPlayer;
    Stone currentTurn;
    QLabel* turnLabel;
    QLabel* blackCountLabel;
    QLabel* whiteCountLabel;
    QLabel* turnIconLabel;


    void flipStones(int x, int y, Stone turn);
    void updateStatus();

    QVector<QVector<Stone>> board;
    QVector<QPoint> validMoves;
private slots:
    void handleCellClick(int x, int y);
};
