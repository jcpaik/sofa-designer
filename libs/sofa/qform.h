#pragma once

#include <vector>

#include "number.h"

class LinearForm;
class CerealReader;

class QuadraticForm {
  public:
    // Constructors
    QuadraticForm();
    explicit QuadraticForm(int d);
    QuadraticForm(const LinearForm &l);
    QuadraticForm(const QT &w0, const std::vector<QT> &w1, 
                  const std::vector< std::vector<QT> > &w2); 

    // Accessors
    int d() const;

    QT &w0();
    const QT &w0() const;

    std::vector<QT> &w1();
    const std::vector<QT> &w1() const;
    QT &w1(int i);
    const QT &w1(int i) const;

    std::vector< std::vector<QT> > &w2();
    const std::vector< std::vector<QT> > &w2() const;
    // when i < j, w2(i, j) is an alias of w2(j, i)
    QT &w2(int i, int j);
    const QT &w2(int i, int j) const;

    // Arithmetic
    QuadraticForm &operator+=(const QuadraticForm &other);
    QuadraticForm &operator-=(const QuadraticForm &other);
    QuadraticForm &operator*=(const QT &c);
    QuadraticForm &operator/=(const QT &c);

    QuadraticForm operator+(const QuadraticForm &other) const;
    QuadraticForm operator-(const QuadraticForm &other) const;
    QuadraticForm operator*(const QT &c) const;
    QuadraticForm operator/(const QT &c) const;

    QuadraticForm operator-() const;

    bool operator==(const QuadraticForm &other) const;
    bool operator!=(const QuadraticForm &other) const;

    QuadraticForm &normalize();

    // Substitution
    QT operator()(std::vector<QT> values) const;

    // Serialization
    // Only this function can change the dimension d()
    friend CerealReader &operator>>(CerealReader &in, QuadraticForm &v);

  private:
    int d_;
    QT w0_;
    std::vector<QT> w1_;
    std::vector< std::vector<QT> > w2_;
};

// Arithmetic
QuadraticForm operator*(const QT &c, const QuadraticForm &f);

// ------------------ inline function definitions ---------------- //

inline int QuadraticForm::d() const {
  return d_;
}

inline QT &QuadraticForm::w0() {
  return w0_;
}

inline const QT &QuadraticForm::w0() const {
  return w0_;
}

inline std::vector<QT> &QuadraticForm::w1() {
  return w1_;
}

inline const std::vector<QT> &QuadraticForm::w1() const {
  return w1_;
}

inline QT &QuadraticForm::w1(int i) {
  return w1_[i];
}

inline const QT &QuadraticForm::w1(int i) const {
  return w1_[i];
}

inline std::vector< std::vector<QT> > &QuadraticForm::w2() {
  return w2_;
}

inline const std::vector< std::vector<QT> > &QuadraticForm::w2() const {
  return w2_;
}

inline QT &QuadraticForm::w2(int i, int j) {
  return i >= j ? w2_[i][j] : w2_[j][i];
}

inline const QT &QuadraticForm::w2(int i, int j) const {
  return i >= j ? w2_[i][j] : w2_[j][i];
}

inline QuadraticForm QuadraticForm::operator+(
    const QuadraticForm &other) const {
  QuadraticForm res(*this);
  res += other;
  return res;
}

inline QuadraticForm QuadraticForm::operator-(
    const QuadraticForm &other) const {
  QuadraticForm res(*this);
  res -= other;
  return res;
}

inline QuadraticForm QuadraticForm::operator*(const QT &c) const {
  QuadraticForm res(*this);
  res *= c;
  return res;
}

inline QuadraticForm QuadraticForm::operator/(const QT &c) const {
  QuadraticForm res(*this);
  res /= c;
  return res;
}

inline QuadraticForm QuadraticForm::operator-() const {
  return (*this) * -1;
}

inline bool QuadraticForm::operator==(const QuadraticForm &other) const {
  return w0_ == other.w0_ && w1_ == other.w1_ && w2_ == other.w2_;
}

inline bool QuadraticForm::operator!=(const QuadraticForm &other) const {
  return w0_ != other.w0_ || w1_ != other.w1_ || w2_ != other.w2_;
}

inline QuadraticForm operator*(const QT &c, const QuadraticForm &f) {
  return f * c;
}
