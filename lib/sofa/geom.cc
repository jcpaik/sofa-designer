#include "geom.h"

#include "expect.h"

// LinearFormPoint

LinearFormPoint::LinearFormPoint() = default;

LinearFormPoint::LinearFormPoint(int d) : x(d), y(d) {
}

LinearFormPoint::LinearFormPoint(
  const LinearForm &x, 
  const LinearForm &y) 
    : x(x), y(y) {
  expect(x.d() == y.d());
}

int LinearFormPoint::d() const { 
  return x.d(); 
}

LinearFormPoint &LinearFormPoint::operator+=(
    const LinearFormPoint &other) {
  x += other.x; y += other.y;
  return *this;
}

LinearFormPoint &LinearFormPoint::operator-=(
    const LinearFormPoint &other) {
  x -= other.x; y -= other.y;
  return *this;
}

LinearFormPoint &LinearFormPoint::operator+=(
    const Vector &other) {
  x += LinearForm::constant(d(), other.x); 
  y += LinearForm::constant(d(), other.y);
  return *this;
}

LinearFormPoint &LinearFormPoint::operator-=(
    const Vector &other) {
  x -= LinearForm::constant(d(), other.x); 
  y -= LinearForm::constant(d(), other.y);
  return *this;
}

LinearFormPoint &LinearFormPoint::operator*=(const QT &c) {
  x *= c; y *= c;
  return *this;
}

LinearFormPoint &LinearFormPoint::operator/=(const QT &c) {
  x /= c; y /= c;
  return *this;
}

LinearFormPoint LinearFormPoint::operator+(
    const LinearFormPoint &other) const {
  return {x + other.x, y + other.y};
}

LinearFormPoint LinearFormPoint::operator-(
    const LinearFormPoint &other) const {
  return {x - other.x, y - other.y};
}

LinearFormPoint LinearFormPoint::operator+(
    const Vector &other) const {
  LinearFormPoint res(*this);
  res += other;
  return res;
}

LinearFormPoint LinearFormPoint::operator-(
    const Vector &other) const {
  LinearFormPoint res(*this);
  res -= other;
  return res;
}

LinearFormPoint LinearFormPoint::operator*(const QT &c) const {
  return {x * c, y * c};
}

LinearFormPoint LinearFormPoint::operator/(const QT &c) const {
  return {x / c, y / c};
}

LinearFormPoint LinearFormPoint::operator-() const {
  return {-x, -y};
}

bool LinearFormPoint::operator==(const LinearFormPoint &other) const {
  return x == other.x && y == other.y;
}

bool LinearFormPoint::operator!=(const LinearFormPoint &other) const {
  return x != other.x || y != other.y;
}

Vector LinearFormPoint::operator()(const std::vector<QT> &v) const {
  return {x(v), y(v)};
}

LinearFormPoint operator*(const QT &c, const LinearFormPoint &a) {
  return a * c;
}

// Vector

Vector::Vector() = default;

Vector::Vector(const QT &x, const QT &y) : x(x), y(y) {
}

Vector &Vector::operator+=(const Vector &other) {
  x += other.x; y += other.y;
  return *this;
}

Vector &Vector::operator-=(const Vector &other) {
  x -= other.x; y -= other.y;
  return *this;
}

Vector Vector::operator+(const Vector &other) const {
  return {x + other.x, y + other.y};
}

Vector Vector::operator-(const Vector &other) const {
  return {x - other.x, y - other.y};
}

LinearFormPoint Vector::operator*(const LinearForm &c) const {
  return {x * c, y * c};
}

Vector Vector::operator-() const {
  return {-x, -y};
}

QT Vector::norm_squared() const {
  return x * x + y * y;
}

bool Vector::operator==(const Vector &other) const {
  return x == other.x && y == other.y;
}

bool Vector::operator!=(const Vector &other) const {
  return x != other.x || y != other.y;
}

LinearFormPoint operator*(const LinearForm &c, const Vector &v) {
  return {c * v.x, c * v.y};
}

QT dot(const Vector &a, const Vector &b) {
  return a.x * b.x + a.y * b.y;
}

LinearForm dot(const LinearFormPoint &a, const Vector &b) {
  return a.x * b.x + a.y * b.y;
}

LinearForm dot(const Vector &a, const LinearFormPoint &b) {
  return a.x * b.x + a.y * b.y;
}

QuadraticForm dot(const LinearFormPoint &a, const LinearFormPoint &b) {
  return a.x * b.x + a.y * b.y;
}

LinearFormPoint intersection(const Line &l1, const Line &l2) {
  // l1.a.x * x + l1.a.y * y == l1.b
  // l2.a.x * x + l2.a.y * y == l2.b
  LinearFormPoint p = LinearFormPoint(
    l2.a.y * l1.b - l1.a.y * l2.b,
    l1.a.x * l2.b - l2.a.x * l1.b
  ) / (l1.a.x * l2.a.y - l2.a.x * l1.a.y);
  expect(dot(p, l1.a) == l1.b && dot(p, l2.a) == l2.b);
  return p;
}

QuadraticForm polygon_area(const std::vector<LinearFormPoint> &points) {
  int n = points.size();
  expect(n > 0);
  int d = points.front().d();
  QuadraticForm area(d);
  for (int i = 0; i < n; i++) {
    expect(points[i].d() == d);
    const auto &cur = points[i];
    const auto &nxt = points[(i + 1) % n];
    area += cur.x * nxt.y - cur.y * nxt.x;
  }
  return area / 2;
}

