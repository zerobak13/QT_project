#pragma once

#include <QWidget>
#include <QVector>
#include <QPoint>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QEvent>  //  QEvent ���� ������

#include "Stone.h"

class ReplayWindow : public QWidget
{
    Q_OBJECT

public:
    ReplayWindow(const QVector<QVector<QVector<Stone>>>& boards,
        const QVector<QPoint>& moves,
        const QVector<Stone>& turns,
        QWidget* parent = nullptr);
signals:
    void requestReturnToMain();  // ����ȭ�� ��û

protected:
    //  eventFilter �Լ� ���� �߰�
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void goToTurn(int index);
    void prevTurn();
    void nextTurn();

private:
    QVector<QVector<QVector<Stone>>> replayBoards;
    QVector<QPoint> replayMoves;
    QVector<Stone> replayTurns;
    int boardSize;
    int currentTurnIndex;

    QWidget* boardArea;
    QLabel* turnLabel;
    QLabel* countLabel;
    QSlider* turnSlider;
    QPushButton* prevButton;
    QPushButton* nextButton;
    QPushButton* mainMenuButton;
    void renderBoard();
};
