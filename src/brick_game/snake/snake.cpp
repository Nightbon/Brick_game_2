#include "snake.h"

namespace s21 {

Snake::Snake()
    : direction_(Direction::LEFT), next_direction_(Direction::LEFT) {}

void Snake::initialize(const Point& start_pos) {
  clear();
  direction_ = Direction::RIGHT;
  next_direction_ = Direction::RIGHT;

  // Создаем начальную змейку (голова + 3 сегмента тела)
  for (int i = 0; i < 4; ++i) {
    body_.push_back(Point(start_pos.x - i, start_pos.y));
  }
}

void Snake::setDirection(Direction dir) {
  if (isValidDirectionChange(dir)) {
    next_direction_ = dir;
  }
}

bool Snake::move(bool grow) {
  // Обновляем текущее направление
  direction_ = next_direction_;

  // Получаем новую позицию головы
  Point new_head = getHead() + getDirectionVector(direction_);

  if (contains(new_head)) {
    return false;  // Столкновение с собой
  }

  // Добавляем новую голову
  body_.push_front(new_head);

  // Если не растем, удаляем хвост
  if (!grow) {
    body_.pop_back();
  }

  return true;
}

bool Snake::checkWallCollision(const Field& field) const {
  return !field.isInside(getHead());
}

bool Snake::contains(const Point& point) const {
  return std::find(body_.begin(), body_.end() - 1, point) != body_.end() - 1;
}

void Snake::clear() { body_.clear(); }

Point Snake::getDirectionVector(Direction dir) const {
  switch (dir) {
    case Direction::UP:
      return Point(0, -1);
    case Direction::RIGHT:
      return Point(1, 0);
    case Direction::DOWN:
      return Point(0, 1);
    case Direction::LEFT:
      return Point(-1, 0);
    default:               // LCOV_EXCL_LINE
      return Point(1, 0);  // LCOV_EXCL_LINE
  }
}

bool Snake::isValidDirectionChange(Direction new_dir) const {
  // Нельзя двигаться в противоположном направлении
  if ((direction_ == Direction::UP && new_dir == Direction::DOWN) ||
      (direction_ == Direction::DOWN && new_dir == Direction::UP) ||
      (direction_ == Direction::LEFT && new_dir == Direction::RIGHT) ||
      (direction_ == Direction::RIGHT && new_dir == Direction::LEFT)) {
    return false;
  }
  return true;
}

}  // namespace s21