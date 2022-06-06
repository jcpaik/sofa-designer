#pragma once

#include <vector>

#include "number.h"

class QuadraticForm;
class CerealReader;

class LinearForm {
  public:
    // Constructors
    LinearForm();
    explicit LinearForm(int d);
    LinearForm(const QT &w0, const std::vector<QT> &w1);

    // Factory methods
    static LinearForm constant(int d, const QT &c);
    static LinearForm variable(int d, int i);

    // Accessors
    int d() const;

    QT &w0();
    const QT &w0() const;

    std::vector<QT> &w1();
    const std::vector<QT> &w1() const;
    QT &w1(int i);
    const QT &w1(int i) const;

    // Arithmetic
    LinearForm &operator+=(const LinearForm &other);
    LinearForm &operator-=(const LinearForm &other);
    LinearForm &operator*=(const QT &c);
    LinearForm &operator/=(const QT &c);

    LinearForm operator+(const LinearForm &other) const;
    LinearForm operator-(const LinearForm &other) const;
    LinearForm operator*(const QT &c) const;
    LinearForm operator/(const QT &c) const;

    QuadraticForm operator*(const LinearForm &other) const;

    LinearForm operator-() const;

    bool operator==(const LinearForm &other) const;
    bool operator!=(const LinearForm &other) const;

    LinearForm &normalize();

    // Substitution
    QT operator()(std::vector<QT> values) const;

    // Serialization
    // Only this function can change the dimension d()
    friend CerealReader &operator>>(CerealReader &in, LinearForm &v);

  private:
    int d_;
    QT w0_;
    std::vector<QT> w1_;
};

// Arithmetic
LinearForm operator*(const QT &c, const LinearForm &f);

// ------------------ inline function definitions ---------------- //

inline LinearForm LinearForm::constant(int d, const QT &c) {
  LinearForm con(d);
  con.w0() = c;
  return con;
}

inline LinearForm LinearForm::variable(int d, int i) {
  LinearForm var(d);
  var.w1(i) = 1;
  return var;
}

inline int LinearForm::d() const {
  return d_;
}

inline QT &LinearForm::w0() {
  return w0_;
}

inline const QT &LinearForm::w0() const {
  return w0_;
}

inline std::vector<QT> &LinearForm::w1() {
  return w1_;
}

inline const std::vector<QT> &LinearForm::w1() const {
  return w1_;
}

inline QT &LinearForm::w1(int i) {
  return w1_[i];
}

inline const QT &LinearForm::w1(int i) const {
  return w1_[i];
}

inline LinearForm LinearForm::operator+(
    const LinearForm &other) const {
  LinearForm res(*this);
  res += other;
  return res;
}

inline LinearForm LinearForm::operator-(
    const LinearForm &other) const {
  LinearForm res(*this);
  res -= other;
  return res;
}

inline LinearForm LinearForm::operator*(const QT &c) const {
  LinearForm res(*this);
  res *= c;
  return res;
}

inline LinearForm LinearForm::operator/(const QT &c) const {
  LinearForm res(*this);
  res /= c;
  return res;
}

inline LinearForm LinearForm::operator-() const {
  return (*this) * -1;
}

inline bool LinearForm::operator==(const LinearForm &other) const {
  return w0_ == other.w0_ && w1_ == other.w1_;
}

inline bool LinearForm::operator!=(const LinearForm &other) const {
  return w0_ != other.w0_ || w1_ != other.w1_;
}

inline LinearForm operator*(const QT &c, const LinearForm &f) {
  return f * c;
}

