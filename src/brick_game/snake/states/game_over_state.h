#ifndef GAME_OVER_STATE_H
#define GAME_OVER_STATE_H

#include "../game_state.h"
#include "playing_state.h"

namespace s21 {

class SnakeGame;

class GameOverState : public GameState {
 public:
  explicit GameOverState(bool win) : is_win_(win) {}

  void handleInput(SnakeGame& game, UserAction_t action) override;
  void update(SnakeGame& game) override;
  GameInfo_t getGameInfo(const SnakeGame& game) const override;

 private:
  bool is_win_;
};

}  // namespace s21

#endif  // GAME_OVER_STATE_H