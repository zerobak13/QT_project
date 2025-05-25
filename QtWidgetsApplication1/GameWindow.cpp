#include "GameWindow.h"
#include "BoardWidget.h"

#include "Stone.h"
#include <QRandomGenerator>
#include <QMessageBox>
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
    while (placed < obstacleCount / 2) {
        int x = QRandomGenerator::global()->bounded(boardSize / 2);  // 왼쪽 절반만 뽑음
        int y = QRandomGenerator::global()->bounded(boardSize);

        int mirrorX = boardSize - 1 - x;

        // 중앙 4칸은 제외
        if ((x == mid || x == mid - 1) && (y == mid || y == mid - 1)) continue;
        if ((mirrorX == mid || mirrorX == mid - 1) && (y == mid || y == mid - 1)) continue;

        if (board[y][x] == Stone::None && board[y][mirrorX] == Stone::None) {
            board[y][x] = Stone::Block;
            board[y][mirrorX] = Stone::Block;
            placed++;
        }
    }

    // obstacleCount가 홀수인 경우 중앙 아래쪽에 하나만 더
    if (obstacleCount % 2 == 1) {
        for (int attempt = 0; attempt < 100; ++attempt) {
            int x = QRandomGenerator::global()->bounded(boardSize);
            int y = QRandomGenerator::global()->bounded(boardSize);

            if ((x == mid || x == mid - 1) && (y == mid || y == mid - 1)) continue;
            if (board[y][x] == Stone::None) {
                board[y][x] = Stone::Block;
                break;
            }
        }
    }


// QString → Stone 변환
    if (firstPlayer == "흑") {
        currentTurn = Stone::Black;
    }
    else if (firstPlayer == "백") {
        currentTurn = Stone::White;
    }
    else {  // "무작위"인 경우
        int r = QRandomGenerator::global()->bounded(2);  // 0 또는 1
        currentTurn = (r == 0) ? Stone::Black : Stone::White;
    }


 

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
     // 턴 돌색 아이콘도 선공에 따라 설정
    turnIconLabel = new QLabel();
    turnIconLabel->setFixedSize(40, 40);

    QString iconColor = (currentTurn == Stone::Black) ? "black" : "white";
    turnIconLabel->setStyleSheet("background-color: " + iconColor + "; border: 1px solid black;");
   


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

    connect(backButton, &QPushButton::clicked, this, [=]() {
        emit requestReturnToMain(this);  // GameWindow를 닫고 메인으로
        });
    // 마지막에 턴 표시, 착수 위치 갱신
    updateValidMoves(currentTurn);
    updateStatus();

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

    QString turnStr = QString("턴 %1: ").arg(turnCount);
    turnStr += (currentTurn == Stone::Black) ? "흑 턴" : "백 턴";

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

    replayBoards.append(board);
    replayMoves.append(QPoint(x, y));
    replayTurns.append(currentTurn == Stone::Black ? Stone::White : Stone::Black);

    // 턴 변경
    currentTurn = (currentTurn == Stone::Black) ? Stone::White : Stone::Black;
    turnCount++;

    // 착수 위치 갱신
    updateValidMoves(currentTurn);
    updateStatus();

    // ✅ 둘 곳이 없으면 패스 or 종료 판단
    if (validMoves.isEmpty()) {
        // 상대방도 못 두면 → 게임 종료
        QVector<QPoint> otherMoves;
        Stone other = (currentTurn == Stone::Black) ? Stone::White : Stone::Black;
        getValidMoves(other, otherMoves);

        if (otherMoves.isEmpty()) {
            // ✅ 양쪽 모두 못 둠 → 바로 게임 종료
            checkGameEndAndNotify();
            return;
        }

        // ✅ 상대방은 둘 수 있음 → 패스 처리
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("패스");
        msgBox.setText("착수 가능한 자리가 없어 턴을 넘깁니다.");
        
        msgBox.setStyleSheet(
            "QLabel { color: black; } "
            "QPushButton { color: black; } "
            "QMessageBox { background-color: white; }"
        );
        msgBox.exec();

      
        // 리플레이 기록 (착수하지 않고 패스만 됨)
        replayBoards.append(board);
        replayMoves.append(QPoint(-1, -1));  // -1, -1: 착수 안함
        replayTurns.append(currentTurn);

        // 턴 넘기기
        currentTurn = other;
        updateValidMoves(currentTurn);
        updateStatus();
      

        // 다시 종료 조건 확인
        checkGameEndAndNotify();
        return;
    }

    // ✅ 게임 종료 조건 확인
    
  
    checkGameEndAndNotify();
}



bool GameWindow::isGameOver()
{
    QVector<QPoint> blackMoves, whiteMoves;
    getValidMoves(Stone::Black, blackMoves);
    getValidMoves(Stone::White, whiteMoves);

    return blackMoves.isEmpty() && whiteMoves.isEmpty();
}




void GameWindow::getValidMoves(Stone turn, QVector<QPoint>& moves)
{
    moves.clear();
    int dx[8] = { -1,-1,-1,0,1,1,1,0 };
    int dy[8] = { -1,0,1,1,1,0,-1,-1 };

    for (int y = 0; y < boardSize; ++y) {
        for (int x = 0; x < boardSize; ++x) {
            if (board[y][x] != Stone::None) continue;

            bool isValid = false;
            for (int d = 0; d < 8; ++d) {
                int nx = x + dx[d];
                int ny = y + dy[d];
                bool foundOpponent = false;

                while (nx >= 0 && nx < boardSize && ny >= 0 && ny < boardSize) {
                    if (board[ny][nx] == Stone::None || board[ny][nx] == Stone::Block)
                        break;

                    if (board[ny][nx] != turn) {
                        foundOpponent = true;
                        nx += dx[d];
                        ny += dy[d];
                    }
                    else {
                        if (foundOpponent)
                            isValid = true;
                        break;
                    }
                }

                if (isValid) break;
            }

            if (isValid)
                moves.append(QPoint(x, y));
        }
    }
}


void GameWindow::checkGameEndAndNotify()
{
    if (!isGameOver())
        return;
    // 게임 종료 직전 상태 기록 (최종 턴)
    replayBoards.append(board);
    replayMoves.append(QPoint(-1, -1));  // 착수 안함
    replayTurns.append(currentTurn);    // 마지막 턴 주인

    int black = 0, white = 0;
    for (const auto& row : board) {
        for (Stone s : row) {
            if (s == Stone::Black) ++black;
            else if (s == Stone::White) ++white;
        }
    }

    QString result;
    if (black > white) result = "흑 승!";
    else if (white > black) result = "백 승!";
    else result = "무승부!";
    isGameOverFlag = true;
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("게임 종료");
    msgBox.setText(result);
    msgBox.setStyleSheet(
        "QLabel { color: black; } "
        "QPushButton { color: black; } "
        "QMessageBox { background-color: white; }"
    );
    msgBox.exec();


   
        QMessageBox saveBox(this);
    saveBox.setWindowTitle("리플레이 저장");
    saveBox.setText("리플레이를 저장하시겠습니까?");
    saveBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    saveBox.setStyleSheet(
        "QLabel { color: black; } "
        "QPushButton { color: black; } "
        "QMessageBox { background-color: white; }"
    );
    QMessageBox::StandardButton reply = (QMessageBox::StandardButton)saveBox.exec();

    if (reply == QMessageBox::Yes) {
        // 나중에 ReplayWindow에서 쓸 수 있도록 상위로 전달할 예정
        // 예: emit replaySaved(replayBoards, replayMoves, replayTurns);
        // 지금은 일단 저장했다는 사실만 알려주기
        qDebug() << " 리플레이 저장 선택됨";
    }
    else {
        // 저장 안 함
        replayBoards.clear();
        replayMoves.clear();
        replayTurns.clear();
        qDebug() << " 리플레이 저장하지 않음";
    }


    //  글자와 버튼 모두 검정색으로
    msgBox.setStyleSheet(
        "QLabel { color: black; } "
        "QPushButton { color: black; } "
        "QMessageBox { background-color: white; }"
    );

   


    emit requestReturnToMain(this);  // 메인으로 복귀
}
