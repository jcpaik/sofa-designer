#pragma once

#include <cassert>
#include <vector>

#include "number.h"
#include "forms.h"
#include "ineq.h"

struct Vector;

// LinearFormPoint

struct LinearFormPoint {
  LinearForm x, y;

  LinearFormPoint();
  LinearFormPoint(int d);
  LinearFormPoint(const LinearForm &x, const LinearForm &y);

  int d() const;

  LinearFormPoint &operator+=(const LinearFormPoint &other);
  LinearFormPoint &operator-=(const LinearFormPoint &other);
  LinearFormPoint &operator+=(const Vector &other);
  LinearFormPoint &operator-=(const Vector &other);
  LinearFormPoint &operator*=(const QT &c);
  LinearFormPoint &operator/=(const QT &c);

  LinearFormPoint operator+(const LinearFormPoint &other) const;
  LinearFormPoint operator-(const LinearFormPoint &other) const;
  LinearFormPoint operator+(const Vector &other) const;
  LinearFormPoint operator-(const Vector &other) const;
  LinearFormPoint operator*(const QT &c) const;
  LinearFormPoint operator/(const QT &c) const;

  LinearFormPoint operator-() const;

  bool operator==(const LinearFormPoint &other) const;
  bool operator!=(const LinearFormPoint &other) const;

  Vector operator()(const std::vector<QT> &v) const;
};

LinearFormPoint operator*(const QT &c, const LinearFormPoint &a);

// Vector

struct Vector {
  QT x, y;

  Vector();
  Vector(const QT &x, const QT &y);

  Vector &operator+=(const Vector &other);
  Vector &operator-=(const Vector &other);

  Vector operator+(const Vector &other) const;
  Vector operator-(const Vector &other) const;

  Vector operator-() const;

  LinearFormPoint operator*(const LinearForm &c) const;
  QT norm_squared() const;
  bool operator==(const Vector &other) const;
  bool operator!=(const Vector &other) const;
};

LinearFormPoint operator*(const LinearForm &c, const Vector &v);

QT dot(const Vector &a, const Vector &b);
LinearForm dot(const LinearFormPoint &a, const Vector &b);
LinearForm dot(const Vector &a, const LinearFormPoint &b);
QuadraticForm dot(const LinearFormPoint &a, const LinearFormPoint &b);

// Line

// Describes a line a.x >= b, with vector `a` and linear form `b`
struct Line {
    Vector a = {0, 1};
    LinearForm b = LinearForm();

    Line() = default;
    Line(const Vector &a, const LinearForm &b) : a(a), b(b) {}
    Line(const Vector &a, const LinearFormPoint &p) : a(a), b(dot(a, p)) {}

    bool operator==(const Line &other) const {
      return a == other.a && b == other.b;
    }
};

LinearFormPoint intersection(const Line &l1, const Line &l2);

QuadraticForm polygon_area(
    const std::vector<LinearFormPoint> &points);

// -------------------------- inlines -------------------------

// LinearFormPoint

inline LinearFormPoint::LinearFormPoint() = default;

inline LinearFormPoint::LinearFormPoint(int d) : x(d), y(d) {
}

inline LinearFormPoint::LinearFormPoint(const LinearForm &x, 
                                        const LinearForm &y) 
    : x(x), y(y) {
  assert(x.d() == y.d());
}

inline int LinearFormPoint::d() const { 
  return x.d(); 
}

inline LinearFormPoint &LinearFormPoint::operator+=(
    const LinearFormPoint &other) {
  x += other.x; y += other.y;
  return *this;
}

inline LinearFormPoint &LinearFormPoint::operator-=(
    const LinearFormPoint &other) {
  x -= other.x; y -= other.y;
  return *this;
}

inline LinearFormPoint &LinearFormPoint::operator+=(
    const Vector &other) {
  x += LinearForm::constant(d(), other.x); 
  y += LinearForm::constant(d(), other.y);
  return *this;
}

inline LinearFormPoint &LinearFormPoint::operator-=(
    const Vector &other) {
  x -= LinearForm::constant(d(), other.x); 
  y -= LinearForm::constant(d(), other.y);
  return *this;
}

inline LinearFormPoint &LinearFormPoint::operator*=(const QT &c) {
  x *= c; y *= c;
  return *this;
}

inline LinearFormPoint &LinearFormPoint::operator/=(const QT &c) {
  x /= c; y /= c;
  return *this;
}

inline LinearFormPoint LinearFormPoint::operator+(
    const LinearFormPoint &other) const {
  return {x + other.x, y + other.y};
}

inline LinearFormPoint LinearFormPoint::operator-(
    const LinearFormPoint &other) const {
  return {x - other.x, y - other.y};
}

inline LinearFormPoint LinearFormPoint::operator+(
    const Vector &other) const {
  LinearFormPoint res(*this);
  res += other;
  return res;
}

inline LinearFormPoint LinearFormPoint::operator-(
    const Vector &other) const {
  LinearFormPoint res(*this);
  res -= other;
  return res;
}

inline LinearFormPoint LinearFormPoint::operator*(const QT &c) const {
  return {x * c, y * c};
}

inline LinearFormPoint LinearFormPoint::operator/(const QT &c) const {
  return {x / c, y / c};
}

inline LinearFormPoint LinearFormPoint::operator-() const {
  return {-x, -y};
}

inline bool LinearFormPoint::operator==(const LinearFormPoint &other) const {
  return x == other.x && y == other.y;
}

inline bool LinearFormPoint::operator!=(const LinearFormPoint &other) const {
  return x != other.x || y != other.y;
}

inline Vector LinearFormPoint::operator()(const std::vector<QT> &v) const {
  return {x(v), y(v)};
}

inline LinearFormPoint operator*(const QT &c, const LinearFormPoint &a) {
  return a * c;
}

// Vector

inline Vector::Vector() = default;

inline Vector::Vector(const QT &x, const QT &y) : x(x), y(y) {
}

inline Vector &Vector::operator+=(const Vector &other) {
  x += other.x; y += other.y;
  return *this;
}

inline Vector &Vector::operator-=(const Vector &other) {
  x -= other.x; y -= other.y;
  return *this;
}

inline Vector Vector::operator+(const Vector &other) const {
  return {x + other.x, y + other.y};
}

inline Vector Vector::operator-(const Vector &other) const {
  return {x - other.x, y - other.y};
}

inline LinearFormPoint Vector::operator*(const LinearForm &c) const {
  return {x * c, y * c};
}

inline Vector Vector::operator-() const {
  return {-x, -y};
}

inline QT Vector::norm_squared() const {
  return x * x + y * y;
}

inline bool Vector::operator==(const Vector &other) const {
  return x == other.x && y == other.y;
}

inline bool Vector::operator!=(const Vector &other) const {
  return x != other.x || y != other.y;
}

inline LinearFormPoint operator*(const LinearForm &c, const Vector &v) {
  return {c * v.x, c * v.y};
}

inline QT dot(const Vector &a, const Vector &b) {
  return a.x * b.x + a.y * b.y;
}

inline LinearForm dot(const LinearFormPoint &a, const Vector &b) {
  return a.x * b.x + a.y * b.y;
}

inline LinearForm dot(const Vector &a, const LinearFormPoint &b) {
  return a.x * b.x + a.y * b.y;
}

inline QuadraticForm dot(const LinearFormPoint &a, const LinearFormPoint &b) {
  return a.x * b.x + a.y * b.y;
}

inline LinearFormPoint intersection(const Line &l1, const Line &l2) {
  // l1.a.x * x + l1.a.y * y == l1.b
  // l2.a.x * x + l2.a.y * y == l2.b
  LinearFormPoint p = LinearFormPoint(
    l2.a.y * l1.b - l1.a.y * l2.b,
    l1.a.x * l2.b - l2.a.x * l1.b
  ) / (l1.a.x * l2.a.y - l2.a.x * l1.a.y);
  assert(dot(p, l1.a) == l1.b && dot(p, l2.a) == l2.b);
  return p;
}

inline QuadraticForm polygon_area(const std::vector<LinearFormPoint> &points) {
  int n = points.size();
  assert(n > 0);
  int d = points.front().d();
  QuadraticForm area(d);
  for (int i = 0; i < n; i++) {
    assert(points[i].d() == d);
    const auto &cur = points[i];
    const auto &nxt = points[(i + 1) % n];
    area += cur.x * nxt.y - cur.y * nxt.x;
  }
  return area / 2;
}

