#pragma once

#include <QtWidgets/QMainWindow>
#include "GameWindow.h"   
#include "ReplayWindow.h"
#include "Stone.h"        
#include <QVector>
#include <QPoint>




class QtWidgetsApplication1 : public QMainWindow
{
    Q_OBJECT
private:
    GameWindow* currentGame = nullptr;
    QVector<QVector<QVector<Stone>>> savedReplayBoards;
    QVector<QPoint> savedReplayMoves;
    QVector<Stone> savedReplayTurns;

public:
    QtWidgetsApplication1(QWidget* parent = nullptr);
    ~QtWidgetsApplication1();


};
