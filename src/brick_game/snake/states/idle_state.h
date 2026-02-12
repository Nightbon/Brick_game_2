#ifndef IDLE_STATE_H
#define IDLE_STATE_H

#include "../game_state.h"
#include "playing_state.h"

namespace s21 {

class SnakeGame;

class IdleState : public GameState {
 public:
  void handleInput(SnakeGame& game, UserAction_t action) override;
  void update(SnakeGame& game) override;
  GameInfo_t getGameInfo(const SnakeGame& game) const override;
};

}  // namespace s21

#endif  // IDLE_STATE_H