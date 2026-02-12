#ifndef FIELD_H
#define FIELD_H

#include <array>

#include "point.h"

namespace s21 {

class Field {
 public:
  static constexpr int WIDTH = 10;
  static constexpr int HEIGHT = 20;

  enum CellType { EMPTY = 0, SNAKE = 1, APPLE = 2, WALL = 3 };

  Field();

  CellType getCell(int x, int y) const;
  void setCell(int x, int y, CellType type);
  void clear();

  bool isInside(const Point& point) const;
  bool isEmpty(const Point& point) const;

  const std::array<std::array<CellType, WIDTH>, HEIGHT>& getGrid() const {
    return grid_;
  }

 private:
  std::array<std::array<CellType, WIDTH>, HEIGHT> grid_{};
};

}  // namespace s21

#endif  // FIELD_H