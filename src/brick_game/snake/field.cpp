#include "field.h"

namespace s21 {

Field::Field() { clear(); }

Field::CellType Field::getCell(int x, int y) const {
  if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
    return grid_[y][x];
  }
  return WALL;  // За границами считаем стеной
}

void Field::setCell(int x, int y, CellType type) {
  if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {  // LCOV_EXCL_LINE
    grid_[y][x] = type;
  }
}

void Field::clear() {
  for (auto& row : grid_) {
    row.fill(EMPTY);
  }
}

bool Field::isInside(const Point& point) const {
  return point.x >= 0 && point.x < WIDTH && point.y >= 0 && point.y < HEIGHT;
}

bool Field::isEmpty(const Point& point) const {
  return isInside(point) && getCell(point.x, point.y) == EMPTY;
}

}  // namespace s21