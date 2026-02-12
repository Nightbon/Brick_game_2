#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "../common.h"
#include "snake_game.h"

namespace s21 {

class SnakeGame;

class GameState {
 public:
  virtual ~GameState() = default;

  virtual void handleInput(SnakeGame& game, UserAction_t action) = 0;
  virtual void update(SnakeGame& game) = 0;
  virtual GameInfo_t getGameInfo(const SnakeGame& game) const = 0;
};

};  // namespace s21

#endif  // GAME_STATE_H