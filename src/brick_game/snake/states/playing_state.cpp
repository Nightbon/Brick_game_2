#include "playing_state.h"

namespace s21 {

void PlayingState::handleInput(SnakeGame& game, UserAction_t action) {
  auto& snake = game.getSnake();

  switch (action) {
    case Left:
      snake.setDirection(Snake::Direction::LEFT);
      break;
    case Right:
      snake.setDirection(Snake::Direction::RIGHT);
      break;
    case Up:
      snake.setDirection(Snake::Direction::UP);
      break;
    case Down:
      snake.setDirection(Snake::Direction::DOWN);
      break;
    case Action:
      update(game);
      break;
    case Pause:
      game.changeState<PausedState>();
      break;
    case Terminate:
      game.reset();
      break;
    default:
      break;
  }
}

void PlayingState::update(SnakeGame& game) {
  timer_counter_++;

  // Рассчитываем задержку в зависимости от скорости
  int speed_delay = BASE_SPEED_DELAY - game.getSpeed();
  if (timer_counter_ < speed_delay) return;

  timer_counter_ = 0;

  auto& snake = game.getSnake();
  auto& apple = game.getApple();
  auto& field = game.getField();

  // Проверяем, съедает ли змейка яблоко
  Snake::Direction dir = snake.getNextDirection();
  bool should_grow = ((snake.getHead() + snake.getDirectionVector(dir)) ==
                      apple.getPosition());

  // Двигаем змейку                   // Столкновение со стеной
  if (!snake.move(should_grow) || snake.checkWallCollision(field)) {
    game.changeState<GameOverState>(false);
    game.updateHighScore();
    return;
  }

  // если should_grow то Обрабатываем съедение яблока
  if (should_grow) {
    game.addScore(SnakeGame::POINTS_PER_APPLE);
    game.updateLevel();
    apple.spawn(field, snake);
  }

  // Проверяем победу
  if (snake.getLength() >= SnakeGame::MAX_SNAKE_LENGTH) {
    game.changeState<GameOverState>(true);
    game.updateHighScore();
    return;
  }

  // Обновляем поле
  field.clear();

  // Отрисовываем змейку
  const auto& body = snake.getBody();
  for (const auto& segment : body) {
    field.setCell(segment.x, segment.y, Field::SNAKE);
  }

  // Отрисовываем яблоко
  const auto& apple_pos = apple.getPosition();
  field.setCell(apple_pos.x, apple_pos.y, Field::APPLE);
}

GameInfo_t PlayingState::getGameInfo(const SnakeGame& game) const {
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
  info.speed = game.getLevel();
  info.pause = 0;

  return info;
}

}  // namespace s21