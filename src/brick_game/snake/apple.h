#ifndef APPLE_H
#define APPLE_H

#include <random>

#include "field.h"
#include "point.h"
#include "snake.h"

namespace s21 {

class Apple {
 public:
  Apple() : rng_(std::random_device{}()) {}

  void spawn(const Field& field, const Snake& snake);
  const Point& getPosition() const { return position_; }
  // cppcheck-suppress unusedFunction
  void setPosition(const Point& point) { position_ = point; }

 private:
  Point position_;
  mutable std::mt19937 rng_;

  Point findValidPosition(const Field& field, const Snake& snake) const;
};

}  // namespace s21

#endif  // APPLE_H