#include "mainwindow.h"

constexpr int widthPixel = 10;
constexpr int heightPixel = 20;
constexpr int pixel = 30;
constexpr int pixelNext = 20;
constexpr int widthField = widthPixel * pixel;
constexpr int heightField = heightPixel * pixel;
constexpr int sizeFieldNext = 4 * pixel;

void setFusionDarkTheme();  // темная тема

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  BrickGameView window;
  window.resize(600, 600);
  window.show();

  return app.exec();
}

BrickGameView::BrickGameView(QWidget *parent) : QMainWindow(parent) {
  setupUI();

  setFusionDarkTheme();  // темная тема

  gameTimer = new QTimer(this);
  controller.handleAction(Action, false);
  connect(gameTimer, &QTimer::timeout, this, &BrickGameView::drawGame);
  gameTimer->start(50);
}

BrickGameView::~BrickGameView() = default;

void BrickGameView::setupUI() {
  centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);

  // Основной layout
  QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
  // Левая панель - игровое поле
  QVBoxLayout *leftLayout = new QVBoxLayout();
  // Правая панель - информация и управление
  QVBoxLayout *rightLayout = new QVBoxLayout();

  // Игровое поле
  gameArea = new QLabel();
  gameArea->setFixedSize(widthField, heightField);

  leftLayout->addWidget(gameArea);

  // Область для следующей фигуры
  nextPieceArea = new QLabel();
  nextPieceArea->setFixedSize(sizeFieldNext, sizeFieldNext);

  scoreLabel = new QLabel("Score: 0");
  highScoreLabel = new QLabel("High Score: 0");
  levelLabel = new QLabel("Level: 1");
  speedLabel = new QLabel("Speed: 1");
  pressStart = new QLabel("PRESS 's' TO START");
  pauseQuit = new QLabel("p - Pause; q - Quit");

  // Добавляем виджеты в правый layout
  rightLayout->addSpacing(50);
  rightLayout->addWidget(scoreLabel);
  rightLayout->addWidget(highScoreLabel);
  rightLayout->addWidget(levelLabel);
  rightLayout->addWidget(speedLabel);
  rightLayout->addSpacing(20);
  rightLayout->addWidget(nextPieceArea);
  rightLayout->addSpacing(300);
  rightLayout->addWidget(pressStart);
  rightLayout->addWidget(pauseQuit);
  rightLayout->addStretch();

  mainLayout->addLayout(leftLayout);
  mainLayout->addLayout(rightLayout);
}

void setFusionDarkTheme() {  // темная тема
  QPalette darkPalette;

  darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
  darkPalette.setColor(QPalette::WindowText, Qt::white);

  qApp->setPalette(darkPalette);
}

void BrickGameView::keyPressEvent(QKeyEvent *event) {
  UserAction_t action = Up;  // По умолчанию

  switch (event->key()) {
    case Qt::Key_Left:
      action = Left;
      break;
    case Qt::Key_Right:
      action = Right;
      break;
    case Qt::Key_Up:
      action = Up;
      break;
    case Qt::Key_Down:
      action = Down;
      break;
    case Qt::Key_Space:
      action = Action;
      break;
    case Qt::Key_P:
      action = Pause;
      break;
    case Qt::Key_S:
      action = Start;
      break;
    case Qt::Key_Q:
      close();
      return;
    default:
      QMainWindow::keyPressEvent(event);
      return;
  }

  controller.handleAction(action, false);
  event->accept();
}

void BrickGameView::drawGame() {
  QPixmap pixmap(widthField, heightField);
  pixmap.fill(Qt::black);

  QPainter painter(&pixmap);
  painter.setPen(QPen(Qt::gray, 1));

  // Отрисовка сетки
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 20; j++) {
      painter.drawRect(i * pixel, j * pixel, pixel, pixel);
    }
  }

  GameInfo_t info = controller.getGameInfo();
  scoreLabel->setText(QString("Score: %1").arg(info.score));
  highScoreLabel->setText(QString("High Score: %1").arg(info.high_score));
  levelLabel->setText(QString("Level: %1").arg(info.level));
  speedLabel->setText(QString("Speed: %1").arg(info.speed));

  painter.setBrush(QBrush(Qt::darkGray));
  for (int i = 0; i < 20; i++) {
    for (int j = 0; j < 10; j++) {
      if (info.field[i][j]) {
        painter.drawRect(j * pixel, i * pixel, pixel, pixel);
      }
    }
  }

  gameArea->setPixmap(pixmap);
  drawNextPiece(info);
}

void BrickGameView::drawNextPiece(const GameInfo_t &info) {
  if (!info.next) return;

  QPixmap nextPixmap(sizeFieldNext, sizeFieldNext);
  nextPixmap.fill(Qt::black);

  QPainter painter(&nextPixmap);
  painter.setPen(QPen(Qt::gray, 1));

  // Центрируем фигуру (4x4)
  static const int offsetX = (sizeFieldNext - 4 * pixel) / 2;
  static const int offsetY = (sizeFieldNext - 4 * pixel) / 2;

  // Отрисовка сетки для следующей фигуры
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      painter.drawRect(offsetX + j * pixel, offsetY + i * pixel, pixel, pixel);
    }
  }

  // Отрисовка следующей фигуры
  painter.setBrush(QBrush(Qt::darkGray));
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (info.next[i][j]) {
        painter.drawRect(offsetX + j * pixel, offsetY + i * pixel, pixel,
                         pixel);
      }
    }
  }

  nextPieceArea->setPixmap(nextPixmap);
}