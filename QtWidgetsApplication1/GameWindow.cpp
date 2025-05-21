#include "GameWindow.h"
#include "BoardWidget.h"

#include "Stone.h"
#include <QRandomGenerator>

#include <QPushButton>
#include <QPainter>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPen>
#include <QDebug>

GameWindow::GameWindow(int boardSize, int obstacleCount, QString firstPlayer, QWidget* parent)
    : QWidget(parent), boardSize(boardSize), obstacleCount(obstacleCount), firstPlayer(firstPlayer)
{
    setWindowTitle("오델로 게임");

    // 보드 상태 초기화
    board = QVector<QVector<Stone>>(boardSize, QVector<Stone>(boardSize, Stone::None));


    // 중앙 돌 배치
    int mid = boardSize / 2;
    board[mid - 1][mid - 1] = Stone::White;
    board[mid - 1][mid] = Stone::Black;
    board[mid][mid - 1] = Stone::Black;
    board[mid][mid] = Stone::White;

    // 장애물 배치 (임의의 빈칸에 obstacleCount만큼)
    int placed = 0;
    while (placed < obstacleCount) {
        int x = QRandomGenerator::global()->bounded(boardSize);
        int y = QRandomGenerator::global()->bounded(boardSize);

        if (board[y][x] == Stone::None && !(x == mid || x == mid - 1) && !(y == mid || y == mid - 1)) {
            board[y][x] = Stone::Block;
            placed++;
        }
    }


    currentTurn = Stone::Black;
    updateValidMoves(currentTurn);

    // ===== BoardWidget 생성 =====
    BoardWidget* boardView = new BoardWidget(boardSize, board, validMoves);
    connect(boardView, &BoardWidget::cellClicked, this, &GameWindow::handleCellClick);

    // ===== 우측 정보창 =====
    turnLabel = new QLabel("턴: 흑 턴");
    blackCountLabel = new QLabel("흑돌: 2");
    whiteCountLabel = new QLabel("백돌: 2");
    //글씨색
    turnLabel->setStyleSheet("color: black;");
    blackCountLabel->setStyleSheet("color: black;");
    whiteCountLabel->setStyleSheet("color: black;");

    QPushButton* backButton = new QPushButton("메인화면");
    backButton->setStyleSheet("color: black;");

    //턴 돌색 표시
    turnIconLabel = new QLabel();
    turnIconLabel->setFixedSize(40, 40);
    turnIconLabel->setStyleSheet("background-color: black; border: 1px solid black;");


    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(turnIconLabel);  // turnLabel 위에 추가
    rightLayout->addWidget(turnLabel);
    rightLayout->addWidget(blackCountLabel);
    rightLayout->addWidget(whiteCountLabel);
    rightLayout->addStretch();
    rightLayout->addWidget(backButton);

    // ===== 전체 레이아웃 구성 =====
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(boardView);
    mainLayout->addLayout(rightLayout);

    setLayout(mainLayout);  

    updateStatus();  // 상태 표시 갱신

    this->setStyleSheet("background-color: #f0f0f0;");

}

GameWindow::~GameWindow() {
    // 필요 시 리소스 정리
}


void GameWindow::updateValidMoves(Stone currentTurn)
{
    validMoves.clear();

    int dx[8] = { -1,-1,-1,0,1,1,1,0 };
    int dy[8] = { -1,0,1,1,1,0,-1,-1 };

    for (int y = 0; y < boardSize; ++y) {
        for (int x = 0; x < boardSize; ++x) {
            if (board[y][x] != Stone::None) continue;  // ❗ Block도 포함됨

            bool isValid = false;

            for (int d = 0; d < 8; ++d) {
                int nx = x + dx[d];
                int ny = y + dy[d];
                bool foundOpponent = false;

                while (nx >= 0 && nx < boardSize && ny >= 0 && ny < boardSize) {
                    if (board[ny][nx] == Stone::None || board[ny][nx] == Stone::Block)
                        break;

                    if (board[ny][nx] != currentTurn) {
                        foundOpponent = true;
                        nx += dx[d];
                        ny += dy[d];
                    }
                    else {
                        if (foundOpponent) {
                            isValid = true;
                        }
                        break;
                    }
                }

                if (isValid) break;
            }

            if (isValid)
                validMoves.append(QPoint(x, y));
        }
    }

    update();  // 화면 갱신
}





void GameWindow::flipStones(int x, int y, Stone turn)
{
    int dx[8] = { -1,-1,-1,0,1,1,1,0 };
    int dy[8] = { -1,0,1,1,1,0,-1,-1 };

    for (int d = 0; d < 8; ++d) {
        int nx = x + dx[d];
        int ny = y + dy[d];
        QVector<QPoint> path;

        while (nx >= 0 && nx < boardSize && ny >= 0 && ny < boardSize) {
            if (board[ny][nx] == Stone::None || board[ny][nx] == Stone::Block)
                break;

            if (board[ny][nx] != turn) {
                path.append(QPoint(nx, ny));
                nx += dx[d];
                ny += dy[d];
            }
            else {
                for (const QPoint& p : path) {
                    board[p.y()][p.x()] = turn;  // 돌 뒤집기
                }
                break;
            }
        }
    }
}

void GameWindow::updateStatus()
{
    int blackCount = 0;
    int whiteCount = 0;

    if (currentTurn == Stone::Black)
        turnIconLabel->setStyleSheet("background-color: black; border: 1px solid black;");
    else
        turnIconLabel->setStyleSheet("background-color: white; border: 1px solid black;");

    for (const auto& row : board) {
        for (Stone s : row) {
            if (s == Stone::Black) ++blackCount;
            else if (s == Stone::White) ++whiteCount;
        }
    }

    QString turnStr = (currentTurn == Stone::Black) ? "턴: 흑 턴" : "턴: 백 턴";
    turnLabel->setText(turnStr);
    blackCountLabel->setText("흑돌: " + QString::number(blackCount));
    whiteCountLabel->setText("백돌: " + QString::number(whiteCount));
}

void GameWindow::handleCellClick(int x, int y)
{
    QPoint clicked(x, y);
    if (!validMoves.contains(clicked)) return;
    if (board[y][x] != Stone::None) return;

    board[y][x] = currentTurn;
    flipStones(x, y, currentTurn);

    currentTurn = (currentTurn == Stone::Black) ? Stone::White : Stone::Black;
    updateValidMoves(currentTurn);
    updateStatus();
}
