#include "apple.h"

namespace s21 {

void Apple::spawn(const Field& field, const Snake& snake) {
  position_ = findValidPosition(field, snake);
}

Point Apple::findValidPosition(const Field& field, const Snake& snake) const {
  std::vector<Point> valid_positions;

  // Собираем все свободные позиции
  for (int y = 0; y < Field::HEIGHT; ++y) {
    for (int x = 0; x < Field::WIDTH; ++x) {
      Point pos(x, y);
      if (field.isEmpty(pos) && !snake.contains(pos)) {
        valid_positions.push_back(pos);
      }
    }
  }

  if (valid_positions.empty()) {
    // Если нет свободных позиций, возвращаем первую клетку
    return Point(0, 0);
  }

  // Выбираем случайную позицию
  std::uniform_int_distribution<> dist(0, valid_positions.size() - 1);
  return valid_positions[dist(rng_)];
}

}  // namespace s21