#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QPainter>
#include <QPen>
#include <QTimer>
#include <QVBoxLayout>

// Включаем общие типы
#include "../../brick_game/common.h"
#include "../../brick_game/controller.h"

class BrickGameView : public QMainWindow {
  Q_OBJECT

 public:
  explicit BrickGameView(QWidget *parent = nullptr);
  ~BrickGameView();

 protected:
  void keyPressEvent(QKeyEvent *event) override;

 private slots:
  void drawGame();

 private:
  void setupUI();
  void drawNextPiece(const GameInfo_t &info);

  // UI элементы
  QWidget *centralWidget;
  QLabel *gameArea;
  QLabel *nextPieceArea;
  QLabel *scoreLabel;
  QLabel *highScoreLabel;
  QLabel *levelLabel;
  QLabel *speedLabel;
  QLabel *pressStart;
  QLabel *pauseQuit;
  QTimer *gameTimer;

  s21::Controller controller;
};

#endif  // MAINWINDOW_H