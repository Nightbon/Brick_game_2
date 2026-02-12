#ifndef POINT_H
#define POINT_H

namespace s21 {

class Point {
 public:
  int x{0};
  int y{0};

  Point() = default;
  Point(int x, int y) : x(x), y(y) {}

  bool operator==(const Point& other) const noexcept {
    return x == other.x && y == other.y;
  }

  bool operator!=(const Point& other) const noexcept {
    return !(*this == other);
  }

  Point& operator+=(const Point& other) noexcept {
    x += other.x;
    y += other.y;
    return *this;
  }

  Point operator+(const Point& other) const noexcept {
    return Point(x + other.x, y + other.y);
  }
};

}  // namespace s21
#endif  // POINT_H