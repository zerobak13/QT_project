#include "QtWidgetsApplication1.h"
#include "GameSettingsDialog.h" 
#include "GameWindow.h" 
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

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

            GameWindow* game = new GameWindow(size, obs, player);
            game->show();
            this->hide();  // 메인창 숨기기 (원하면 유지해도 됨)
        }
        });

    QPushButton* continueButton = new QPushButton("이어하기",this);
    layout->addWidget(continueButton);

    QPushButton* replayButton = new QPushButton("리플레이", this);
    layout->addWidget(replayButton);

    QPushButton* exitButton = new QPushButton("게임 종료", this);
    layout->addWidget(exitButton);

    centralWidget->setLayout(layout);
}

QtWidgetsApplication1::~QtWidgetsApplication1()
{
    // 필요한 정리 작업이 있다면 여기 작성 (예: delete 동적 객체)
}
