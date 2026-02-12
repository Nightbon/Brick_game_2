#ifndef PAUSED_STATE_H
#define PAUSED_STATE_H

#include "../game_state.h"
#include "idle_state.h"
#include "playing_state.h"

namespace s21 {

class SnakeGame;

class PausedState : public GameState {
 public:
  void handleInput(SnakeGame& game, UserAction_t action) override;
  void update(SnakeGame& game) override;
  GameInfo_t getGameInfo(const SnakeGame& game) const override;
};

}  // namespace s21

#endif  // PAUSED_STATE_H