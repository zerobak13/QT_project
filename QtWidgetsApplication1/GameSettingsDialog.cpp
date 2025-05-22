#include "GameSettingsDialog.h"

GameSettingsDialog::GameSettingsDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("게임 설정");

    boardSizeCombo = new QComboBox(this);
    for (int i = 6; i <= 12; i += 2)
        boardSizeCombo->addItem(QString::number(i));

    obstacleSpin = new QSpinBox(this);
    obstacleSpin->setRange(0, 16);

    firstPlayerCombo = new QComboBox(this);
    firstPlayerCombo->addItems({ "흑", "백"});

    QPushButton* okButton = new QPushButton("확인", this);
    QPushButton* cancelButton = new QPushButton("취소", this);

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    QVBoxLayout* mainLayout = new QVBoxLayout();

    mainLayout->addWidget(new QLabel("보드 크기:"));
    mainLayout->addWidget(boardSizeCombo);

    mainLayout->addWidget(new QLabel("장애물 개수:"));
    mainLayout->addWidget(obstacleSpin);

    mainLayout->addWidget(new QLabel("선공 플레이어:"));
    mainLayout->addWidget(firstPlayerCombo);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

int GameSettingsDialog::getBoardSize() const {
    return boardSizeCombo->currentText().toInt();
}

int GameSettingsDialog::getObstacleCount() const {
    return obstacleSpin->value();
}

QString GameSettingsDialog::getFirstPlayer() const {
    return firstPlayerCombo->currentText();
}
