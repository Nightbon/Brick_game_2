#include "snake_game.h"

#include "states/idle_state.h"

namespace s21 {

SnakeGame::SnakeGame() : state_(std::make_unique<IdleState>()) {
  loadHighScore();
  initializeGame();
}

void SnakeGame::processInput(UserAction_t action) {
  state_->handleInput(*this, action);
}

void SnakeGame::update() { state_->update(*this); }

GameInfo_t SnakeGame::getGameInfo() const { return state_->getGameInfo(*this); }

void SnakeGame::start() {
  reset();

  // Размещаем змейку в центре
  Point start_pos(Field::WIDTH / 2, Field::HEIGHT / 2);
  snake_.initialize(start_pos);

  // Размещаем яблоко
  apple_.spawn(field_, snake_);

  // Обновляем поле
  field_.clear();

  // Отражаем змейку на поле
  const auto& body = snake_.getBody();
  // int segment_index = 0;
  for (const auto& segment : body) {
    field_.setCell(segment.x, segment.y, Field::SNAKE);
  }

  // Отрисовываем яблоко
  const auto& apple_pos = apple_.getPosition();
  field_.setCell(apple_pos.x, apple_pos.y, Field::APPLE);
}

void SnakeGame::reset() {
  score_ = 4;
  level_ = 1;
  speed_ = INITIAL_SPEED;

  initializeGame();
}

void SnakeGame::initializeGame() {
  field_.clear();
  snake_.clear();
}

void SnakeGame::addScore(int points) {
  score_ += points;
  updateHighScore();
}

void SnakeGame::updateLevel() {
  int new_level = (score_ / POINTS_PER_LEVEL) + 1;
  if (new_level > MAX_LEVEL) {
    new_level = MAX_LEVEL;
  }
  if (new_level != level_) {
    level_ = new_level;
    speed_ = INITIAL_SPEED + (level_ * SPEED_INCREMENT);
    if (speed_ > 18) speed_ = 18;
  }
}

void SnakeGame::updateHighScore() {
  if (score_ > high_score_) {
    high_score_ = score_;
    saveHighScore();
  }
}

void SnakeGame::saveHighScore() const {
  std::ofstream file(HIGHSCORE_FILE, std::ios::binary);
  if (file.is_open()) {  // LCOV_EXCL_LINE
    file.write(reinterpret_cast<const char*>(&high_score_),
               sizeof(high_score_));
  }
}

void SnakeGame::loadHighScore() {
  std::ifstream file(HIGHSCORE_FILE, std::ios::binary);
  if (file.is_open()) {
    file.read(reinterpret_cast<char*>(&high_score_), sizeof(high_score_));
  }
}

}  // namespace s21