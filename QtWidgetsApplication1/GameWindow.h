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
    QVector<QVector<QVector<Stone>>> getReplayBoards() const { return replayBoards; }
    QVector<QPoint> getReplayMoves() const { return replayMoves; }
    QVector<Stone> getReplayTurns() const { return replayTurns; }
    bool isGameOverFlag = false;
protected:
    
    void updateValidMoves(Stone currentTurn);  
signals:
    void requestReturnToMain(GameWindow* sender);


private:
    int boardSize;
    int obstacleCount;
    int turnCount = 1;
    QString firstPlayer;
    Stone currentTurn;
    QLabel* turnLabel;
    QLabel* blackCountLabel;
    QLabel* whiteCountLabel;
    QLabel* turnIconLabel;


    void flipStones(int x, int y, Stone turn);
    void updateStatus();
    //게임종료
    bool isGameOver();
    void getValidMoves(Stone turn, QVector<QPoint>& moves);
    void checkGameEndAndNotify();

    QVector<QVector<Stone>> board;
    QVector<QPoint> validMoves;
    QVector<QVector<QVector<Stone>>> replayBoards;  // 보드 상태
    QVector<QPoint> replayMoves;                    // 착수 위치
    QVector<Stone> replayTurns;                     // 누가 뒀는지

private slots:
    void handleCellClick(int x, int y);
};
