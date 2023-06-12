#pragma once

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
