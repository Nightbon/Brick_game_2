#include "idle_state.h"

namespace s21 {

void IdleState::handleInput(SnakeGame& game, UserAction_t action) {
  if (action == Start) {
    game.start();
    game.changeState<PlayingState>();  // LCOV_EXCL_LINE
  }
}

void IdleState::update(SnakeGame& game) {
  if (game.getLevel()) {
  }
  // В режиме ожидания ничего не обновляем
}

GameInfo_t IdleState::getGameInfo(const SnakeGame& game) const {
  GameInfo_t info{};

  static int* rows[Field::HEIGHT];
  static int field_data[Field::HEIGHT][Field::WIDTH];

  const auto& grid = game.getField().getGrid();
  for (int y = 0; y < Field::HEIGHT; ++y) {
    for (int x = 0; x < Field::WIDTH; ++x) {
      field_data[y][x] = static_cast<int>(grid[y][x]);
    }
    rows[y] = field_data[y];
  }

  info.field = rows;
  info.next = nullptr;
  info.score = 0;
  info.high_score = game.getHighScore();
  info.level = 0;
  info.speed = 0;
  info.pause = 0;

  return info;
}

}  // namespace s21