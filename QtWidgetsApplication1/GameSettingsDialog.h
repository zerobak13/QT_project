#pragma once

#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class GameSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    GameSettingsDialog(QWidget* parent = nullptr);

    int getBoardSize() const;
    int getObstacleCount() const;
    QString getFirstPlayer() const;

private:
    QComboBox* boardSizeCombo;
    QSpinBox* obstacleSpin;
    QComboBox* firstPlayerCombo;
};
