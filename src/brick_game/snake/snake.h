#ifndef SNAKE_H
#define SNAKE_H

#include <algorithm>
#include <deque>
#include <vector>

#include "field.h"
#include "point.h"

namespace s21 {

class Snake {
 public:
  enum class Direction { UP, RIGHT, DOWN, LEFT };

  Snake();

  void setDirection(Direction dir);

  const std::deque<Point>& getBody() const { return body_; }
  Direction getNextDirection() const { return next_direction_; }
  const Point& getHead() const { return body_.front(); }
  size_t getLength() const { return body_.size(); }
  Point getDirectionVector(Direction dir) const;

  bool isValidDirectionChange(Direction new_dir) const;
  bool checkWallCollision(const Field& field) const;
  void initialize(const Point& start_pos);
  bool contains(const Point& point) const;
  bool move(bool grow = false);
  void clear();

 private:
  std::deque<Point> body_;
  Direction direction_;
  Direction next_direction_;
};

}  // namespace s21

#endif  // SNAKE_H