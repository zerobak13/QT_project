#include "QtWidgetsApplication1.h"
#include "GameSettingsDialog.h" 
#include "GameWindow.h" 
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QApplication>


QtWidgetsApplication1::QtWidgetsApplication1(QWidget* parent)
    : QMainWindow(parent)
{
    QWidget* centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);

    QVBoxLayout* layout = new QVBoxLayout();

    QLabel* label = new QLabel("202312345 홍길동", this);
    layout->addWidget(label);

    QPushButton* newGameButton = new QPushButton("새 게임", this);
    layout->addWidget(newGameButton);
  
    connect(newGameButton, &QPushButton::clicked, this, [=]() {
        GameSettingsDialog dlg(this);
        if (dlg.exec() == QDialog::Accepted) {
            int size = dlg.getBoardSize();
            int obs = dlg.getObstacleCount();
            QString player = dlg.getFirstPlayer();

            if (currentGame) {
                delete currentGame;  // 이전 게임 있으면 삭제
            }
            currentGame = new GameWindow(size, obs, player);

           
            connect(currentGame, &GameWindow::requestReturnToMain, this, [=]() {
                savedReplayBoards = currentGame->getReplayBoards();
                savedReplayMoves = currentGame->getReplayMoves();
                savedReplayTurns = currentGame->getReplayTurns();
                currentGame->hide();
                this->show();
                });

            currentGame->show();
            this->hide();  // 메인 화면 숨김
        }
        });

    QPushButton* continueButton = new QPushButton("이어하기",this);
    layout->addWidget(continueButton);
    connect(continueButton, &QPushButton::clicked, this, [=]() {
        if (currentGame) {
            currentGame->show();
            this->hide();
        }
        else {
            QMessageBox::information(this, "안내", "이어할 게임이 없습니다.");
        }
        });


    QPushButton* replayButton = new QPushButton("리플레이", this);
    layout->addWidget(replayButton);
    connect(replayButton, &QPushButton::clicked, this, [=]() {
        if (!savedReplayBoards.isEmpty()) {
            ReplayWindow* replayWin = new ReplayWindow(savedReplayBoards, savedReplayMoves, savedReplayTurns);
            // ReplayWindow에서 메인화면 요청 시 연결
            connect(replayWin, &ReplayWindow::requestReturnToMain, this, [=]() {
                this->show();  // 메인화면 다시 표시
                });
            replayWin->show();
            this->hide();
        }
        else {
            QMessageBox::information(this, "안내", "리플레이할 게임 데이터가 없습니다.");
        }
        });


    QPushButton* exitButton = new QPushButton("게임 종료", this);
    layout->addWidget(exitButton);
    connect(exitButton, &QPushButton::clicked, this, []() {
        QApplication::quit();
        });

    centralWidget->setLayout(layout);
}

QtWidgetsApplication1::~QtWidgetsApplication1()
{
    // 필요한 정리 작업이 있다면 여기 작성 (예: delete 동적 객체)
}
