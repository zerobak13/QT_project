#include "ReplayWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QEvent>

ReplayWindow::ReplayWindow(const QVector<QVector<QVector<Stone>>>& boards,
    const QVector<QPoint>& moves,
    const QVector<Stone>& turns,
    QWidget* parent)
    : QWidget(parent),
    replayBoards(boards),
    replayMoves(moves),
    replayTurns(turns),
    currentTurnIndex(0)
{
    boardSize = boards[0].size();
    setWindowTitle("리플레이");

    // 상단 상태창
    turnLabel = new QLabel(this);
    countLabel = new QLabel(this);

    // 보드 영역
    boardArea = new QWidget(this);
    boardArea->setFixedSize(600, 600);
    boardArea->setStyleSheet("background-color: #e0e0e0;");
    boardArea->installEventFilter(this);

    // 슬라이더 및 버튼
    turnSlider = new QSlider(Qt::Horizontal, this);
    turnSlider->setRange(0, boards.size() - 1);

    prevButton = new QPushButton("이전", this);
    nextButton = new QPushButton("다음", this);

    connect(turnSlider, &QSlider::valueChanged, this, &ReplayWindow::goToTurn);
    connect(prevButton, &QPushButton::clicked, this, &ReplayWindow::prevTurn);
    connect(nextButton, &QPushButton::clicked, this, &ReplayWindow::nextTurn);


    mainMenuButton = new QPushButton("메인화면", this);
    connect(mainMenuButton, &QPushButton::clicked, this, [=]() {
        emit requestReturnToMain();  // 메인화면으로 가라는 시그널 전송
        this->close();               // ReplayWindow 창 닫기
        });


    // 레이아웃 구성
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(turnLabel);
    mainLayout->addWidget(countLabel);
    mainLayout->addWidget(boardArea);

    QHBoxLayout* controlLayout = new QHBoxLayout();
    controlLayout->addWidget(prevButton);
    controlLayout->addWidget(turnSlider);
    controlLayout->addWidget(nextButton);
    controlLayout->addWidget(mainMenuButton);

    mainLayout->addLayout(controlLayout);
    setLayout(mainLayout);

    renderBoard();
}


void ReplayWindow::goToTurn(int index)
{
    if (index < 0 || index >= replayBoards.size()) return;
    currentTurnIndex = index;
    renderBoard();
}

void ReplayWindow::prevTurn()
{
    if (currentTurnIndex > 0) {
        currentTurnIndex--;
        turnSlider->setValue(currentTurnIndex);
        renderBoard();
    }
}

void ReplayWindow::nextTurn()
{
    if (currentTurnIndex < replayBoards.size() - 1) {
        currentTurnIndex++;
        turnSlider->setValue(currentTurnIndex);
        renderBoard();
    }
}

void ReplayWindow::renderBoard()
{
    Stone turn = replayTurns[currentTurnIndex];
    QString turnStr = (turn == Stone::Black) ? "흑" : "백";
    turnLabel->setText("턴" + QString::number(currentTurnIndex + 1) + " : " + turnStr);
    int black = 0, white = 0;
    for (const auto& row : replayBoards[currentTurnIndex]) {
        for (Stone s : row) {
            if (s == Stone::Black) ++black;
            else if (s == Stone::White) ++white;
        }
    }
    countLabel->setText("흑돌: " + QString::number(black) + "백돌: " + QString::number(white));
    boardArea->update();
}

bool ReplayWindow::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == boardArea && event->type() == QEvent::Paint) {
        QPainter painter(boardArea);
        int cellSize = boardArea->width() / boardSize;

        const auto& board = replayBoards[currentTurnIndex];

        QPen pen(Qt::black);
        pen.setWidth(2);
        painter.setPen(pen);

        for (int i = 0; i <= boardSize; ++i) {
            painter.drawLine(i * cellSize, 0, i * cellSize, boardArea->height());
            painter.drawLine(0, i * cellSize, boardArea->width(), i * cellSize);
        }

        for (int y = 0; y < boardSize; ++y) {
            for (int x = 0; x < boardSize; ++x) {
                QRect cell(x * cellSize + 5, y * cellSize + 5, cellSize - 10, cellSize - 10);
                if (board[y][x] == Stone::Black) {
                    painter.setBrush(Qt::black);
                    painter.drawEllipse(cell);
                }
                else if (board[y][x] == Stone::White) {
                    painter.setBrush(Qt::white);
                    painter.setPen(Qt::black);
                    painter.drawEllipse(cell);
                }
                else if (board[y][x] == Stone::Block) {
                    painter.setBrush(Qt::darkBlue);
                    painter.setPen(Qt::black);
                    painter.drawRect(x * cellSize + 1, y * cellSize + 1, cellSize - 2, cellSize - 2);
                }
            }
        }

        // 착수 위치 강조
        QPoint move = replayMoves[currentTurnIndex];
        QRect highlight(move.x() * cellSize + 2, move.y() * cellSize + 2, cellSize - 4, cellSize - 4);
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(Qt::red, 3));
        painter.drawRect(highlight);

        return true;
    }

    return QWidget::eventFilter(obj, event);
}
