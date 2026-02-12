#include "snake_game.h"

static s21::SnakeGame game_instance;

extern "C" {

GameInfo_t updateCurrentState() {
  game_instance.update();
  return game_instance.getGameInfo();
}

void userInput(UserAction_t action, bool hold) {
  if (!hold) {
    game_instance.processInput(action);
  }
}

}  // extern "C"