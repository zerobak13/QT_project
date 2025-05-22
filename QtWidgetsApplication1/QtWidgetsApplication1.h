#pragma once

#include <QtWidgets/QMainWindow>
#include "GameWindow.h"   
#include "Stone.h"        
#include <QVector>



class QtWidgetsApplication1 : public QMainWindow
{
    Q_OBJECT
private:
    GameWindow* currentGame = nullptr;

public:
    QtWidgetsApplication1(QWidget* parent = nullptr);
    ~QtWidgetsApplication1();
};
