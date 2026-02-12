#ifndef SNAKE_GAME_H
#define SNAKE_GAME_H

#include <fstream>
#include <memory>

#include "../common.h"
#include "apple.h"
#include "field.h"
#include "game_state.h"
#include "snake.h"

namespace s21 {

class GameState;

class SnakeGame {
 public:
  friend class GameState;

  static constexpr int MAX_SNAKE_LENGTH = 200;
  static constexpr int POINTS_PER_APPLE = 1;
  static constexpr int POINTS_PER_LEVEL = 5;
  static constexpr int MAX_LEVEL = 10;
  static constexpr int INITIAL_SPEED = 5;
  static constexpr int SPEED_INCREMENT = 2;

  SnakeGame();
  ~SnakeGame() = default;

  // API для C-интерфейса
  void processInput(UserAction_t action);
  void update();
  GameInfo_t getGameInfo() const;

  // LCOV_EXCL_START
  // Смена состояний
  template <typename T, typename... Args>
  void changeState(Args&&... args) {
    state_ = std::make_unique<T>(std::forward<Args>(args)...);
  }
  // LCOV_EXCL_STOP

  int getScore() const { return score_; }
  int getHighScore() const { return high_score_; }
  int getLevel() const { return level_; }
  int getSpeed() const { return speed_; }
  Field& getField() { return field_; }
  Snake& getSnake() { return snake_; }
  Apple& getApple() { return apple_; }
  const Field& getField() const { return field_; }
  const Snake& getSnake() const { return snake_; }
  const Apple& getApple() const { return apple_; }

  void start();
  void reset();
  void addScore(int points);
  void updateLevel();
  void updateHighScore();

 private:
  void initializeGame();
  void saveHighScore() const;
  void loadHighScore();

  Field field_;
  Snake snake_;
  Apple apple_;

  std::unique_ptr<GameState> state_;

  int score_{0};
  int high_score_{0};
  int level_{1};
  int speed_{INITIAL_SPEED};

  static constexpr const char* HIGHSCORE_FILE = "snake_highscore.dat";
};

}  // namespace s21

/**
 * \mainpage Игра "Змейка"
 * \section A Конечный автомат игры "Змейка"
 *
 * \dot
 * digraph SnakeGameStates {
    rankdir=TB;
    node [shape=Mrecord, style=filled, fillcolor=lightblue, fontsize=9];
    edge [fontsize=8];

    // States with more details
    IdleState [label="{IdleState|Ожидание начала игры|• Отображение рекорда\l•
 Ожидание Start\l}", fillcolor=palegreen]; PlayingState
 [label="{PlayingState|Игровой процесс|• Движение змейки\l• Обработка
 столкновений\l• Сбор яблок\l• Обновление счета\l}", fillcolor=lightyellow];
    PausedState [label="{PausedState|Пауза|• Сохранение состояния\l• Ожидание
 снятия паузы\l}", fillcolor=wheat]; GameOverState [label="{GameOverState|Конец
 игры|• Победа/Проигрыш\l• Финальный счет\l• Кнопка перезапуска\l}",
 fillcolor=lightcoral];

    // Transitions with conditions
    IdleState -> PlayingState [label="Start", color=green];
    PlayingState -> PausedState [label="Pause", color=blue];
    PausedState -> PlayingState [label="Pause", color=blue];

    PlayingState -> GameOverState [label="Условие 1:
 Столкновение\n(стена/себя)", color=red]; PlayingState -> GameOverState
 [label="Условие 2: Победа\n(длина ≥ 200)", color=darkgreen];

    GameOverState -> IdleState [label="Start\n(новая игра)", color=purple];

    // Internal transitions in PlayingState
    PlayingState -> PlayingState [label="Внутренние действия:|•
 Left/Right/Up/Down\l• Action\l• Apple eaten\l• Level up\l", style=dashed,
 color=gray, fontcolor=gray];
 * }
 * \enddot
 *
 * \section B Подробное описание переходов
 * \dot
 * digraph TransitionsDetail {
    rankdir=LR;
    node [shape=none, fontsize=8];
    edge [fontsize=7];

    subgraph cluster_playing {
        label="PlayingState - детализация";
        style=filled;
        fillcolor=lightyellow;

        subgraph cluster_input {
            label="Пользовательский ввод";
            fontsize=7;
            style=dashed;

            node [shape=box, width=1.5, height=0.3];
            Left [label="Left (Влево)"];
            Right [label="Right (Вправо)"];
            Up [label="Up (Вверх)"];
            Down [label="Down (Вниз)"];
            Action [label="Action (Обновить)"];
        }

        subgraph cluster_logic {
            label="Игровая логика";
            fontsize=7;
            style=dashed;

            node [shape=ellipse];
            Move [label="Движение змейки"];
            CollisionCheck [label="Проверка столкновений"];
            AppleCheck [label="Проверка яблока"];
            ScoreUpdate [label="Обновление счета"];
        }

        // Connections
        Left -> Move;
        Right -> Move;
        Up -> Move;
        Down -> Move;
        Action -> Move;

        Move -> CollisionCheck;
        CollisionCheck -> AppleCheck [label="Нет столкновения"];
        AppleCheck -> ScoreUpdate [label="Яблоко съедено"];
        ScoreUpdate -> Move [label="Продолжить"];

        CollisionCheck -> GameOver [label="Столкновение", color=red];
        Move -> WinCheck [label="Длина ≥ 200"];
        WinCheck -> GameOver [label="Победа!", color=green];
    }

    GameOver [shape=doublecircle, fillcolor=red];
 * }
 * \enddot
*/

#endif  // SNAKE_GAME_H