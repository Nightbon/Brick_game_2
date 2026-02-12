#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "common.h"  // Общие типы

namespace s21 {

class Controller {
 public:
  Controller() {}

  void handleAction(UserAction_t action, bool hold) { userInput(action, hold); }

  GameInfo_t getGameInfo() const { return updateCurrentState(); }
};

}  // namespace s21

#endif  // CONTROLLER_H