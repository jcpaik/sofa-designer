#include "qform.h"

#include <cassert>

#include "lform.h"

// Constructors

QuadraticForm::QuadraticForm() = default;

QuadraticForm::QuadraticForm(int d)
    : d_(d), w0_(), w1_(d), w2_(d) {
  for (int i = 0; i < d_; i++)
    w2_[i].resize(i + 1);
}

QuadraticForm::QuadraticForm(const LinearForm &l)
    : d_(l.d()), w0_(l.w0()), w1_(l.w1()), w2_(l.d()) {
  for (int i = 0; i < d_; i++)
    w2_[i].resize(i + 1);
}

QuadraticForm::QuadraticForm(
    const QT &w0, 
    const std::vector<QT> &w1,
    const std::vector< std::vector<QT> > &w2)
    : d_(int(w1.size())), w0_(w0), w1_(w1), w2_(int(w1.size())) {
  assert(w2.size() == w1.size());
  for (int i = 0; i < d_; i++) {
    assert(int(w2[i].size()) >= i + 1);
    w2_[i] = std::vector<QT>(w2[i].begin(), w2[i].begin() + i + 1);
  }
}

int QuadraticForm::d() const {
  return d_;
}

QT &QuadraticForm::w0() {
  return w0_;
}

const QT &QuadraticForm::w0() const {
  return w0_;
}

std::vector<QT> &QuadraticForm::w1() {
  return w1_;
}

const std::vector<QT> &QuadraticForm::w1() const {
  return w1_;
}

QT &QuadraticForm::w1(int i) {
  return w1_[i];
}

const QT &QuadraticForm::w1(int i) const {
  return w1_[i];
}

std::vector< std::vector<QT> > &QuadraticForm::w2() {
  return w2_;
}

const std::vector< std::vector<QT> > &QuadraticForm::w2() const {
  return w2_;
}

QT &QuadraticForm::w2(int i, int j) {
  return i >= j ? w2_[i][j] : w2_[j][i];
}

const QT &QuadraticForm::w2(int i, int j) const {
  return i >= j ? w2_[i][j] : w2_[j][i];
}

QuadraticForm &QuadraticForm::operator+=(const QuadraticForm &other) {
  assert(d_ == other.d_);

  w0_ += other.w0_;
  for (int i = 0; i < d_; i++)
    w1_[i] += other.w1_[i];
  for (int i = 0; i < d_; i++)
    for (int j = 0; j <= i; j++)
      w2_[i][j] += other.w2_[i][j];

  return *this;
}

QuadraticForm &QuadraticForm::operator-=(const QuadraticForm &other) {
  assert(d_ == other.d_);

  w0_ -= other.w0_;
  for (int i = 0; i < d_; i++)
    w1_[i] -= other.w1_[i];
  for (int i = 0; i < d_; i++)
    for (int j = 0; j <= i; j++)
      w2_[i][j] -= other.w2_[i][j];

  return *this;
}

QuadraticForm &QuadraticForm::operator*=(const QT &c) {
  w0_ *= c;
  for (int i = 0; i < d_; i++)
    w1_[i] *= c;
  for (int i = 0; i < d_; i++)
    for (int j = 0; j <= i; j++)
      w2_[i][j] *= c;

  return *this;
}

QuadraticForm &QuadraticForm::operator/=(const QT &c) {
  assert(c != 0);
  w0_ /= c;
  for (int i = 0; i < d_; i++)
    w1_[i] /= c;
  for (int i = 0; i < d_; i++)
    for (int j = 0; j <= i; j++)
      w2_[i][j] /= c;

  return *this;
}

QuadraticForm QuadraticForm::operator+(
    const QuadraticForm &other) const {
  QuadraticForm res(*this);
  res += other;
  return res;
}

QuadraticForm QuadraticForm::operator-(
    const QuadraticForm &other) const {
  QuadraticForm res(*this);
  res -= other;
  return res;
}

QuadraticForm QuadraticForm::operator*(const QT &c) const {
  QuadraticForm res(*this);
  res *= c;
  return res;
}

QuadraticForm QuadraticForm::operator/(const QT &c) const {
  QuadraticForm res(*this);
  res /= c;
  return res;
}

QuadraticForm QuadraticForm::operator-() const {
  return (*this) * -1;
}

bool QuadraticForm::operator==(const QuadraticForm &other) const {
  return w0_ == other.w0_ && w1_ == other.w1_ && w2_ == other.w2_;
}

bool QuadraticForm::operator!=(const QuadraticForm &other) const {
  return w0_ != other.w0_ || w1_ != other.w1_ || w2_ != other.w2_;
}

QuadraticForm &QuadraticForm::normalize() {
  w0_.normalize();
  for (auto &v : w1_)
    v.normalize();
  for (auto &row : w2_)
    for (auto &v : row)
      v.normalize();
  return *this;
}

QT QuadraticForm::operator()(std::vector<QT> v) const {
  assert(int(v.size()) == d_);

  QT res = w0_;
  for (int i = 0; i < d_; i++) {
    res += w1_[i] * v[i];
  }
  for (int i = 0; i < d_; i++) {
    for (int j = 0; j < i; j++) {
      res += w2_[i][j] * v[i] * v[j];
    }
    res += w2_[i][i] / 2 * v[i] * v[i];
  }

  return res;
}

QuadraticForm operator*(const QT &c, const QuadraticForm &f) {
  return f * c;
}
