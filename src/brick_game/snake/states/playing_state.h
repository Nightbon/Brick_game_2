#ifndef PLAYING_STATE_H
#define PLAYING_STATE_H

#include "../game_state.h"
#include "game_over_state.h"
#include "paused_state.h"

namespace s21 {

class SnakeGame;

class PlayingState : public GameState {
 public:
  PlayingState() : timer_counter_(0) {}

  void handleInput(SnakeGame& game, UserAction_t action) override;
  void update(SnakeGame& game) override;
  GameInfo_t getGameInfo(const SnakeGame& game) const override;

 private:
  int timer_counter_;
  const int BASE_SPEED_DELAY = 20;  // Базовая задержка
};

}  // namespace s21

#endif  // PLAYING_STATE_H