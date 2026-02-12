#include "paused_state.h"

namespace s21 {

void PausedState::handleInput(SnakeGame& game, UserAction_t action) {
  if (action == Pause) {
    game.changeState<PlayingState>();  // LCOV_EXCL_LINE
  }
}

void PausedState::update(SnakeGame& game) {
  if (game.getLevel()) {
  }
  // На паузе ничего не обновляем
}

GameInfo_t PausedState::getGameInfo(const SnakeGame& game) const {
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
  info.score = game.getScore();
  info.high_score = game.getHighScore();
  info.level = game.getLevel();
  info.speed = game.getSpeed();
  info.pause = 1;

  return info;
}

}  // namespace s21