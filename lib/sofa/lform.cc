#include "lform.h"

#include "qform.h"

LinearForm::LinearForm() = default;

LinearForm::LinearForm(int d) : d_(d), w0_(), w1_(d) {
}

LinearForm::LinearForm(const QT &w0, const std::vector<QT> &w1)
    : d_(int(w1.size())), w0_(w0), w1_(w1) {
}

LinearForm LinearForm::constant(int d, const QT &c) {
  LinearForm con(d);
  con.w0() = c;
  return con;
}

LinearForm LinearForm::variable(int d, int i) {
  LinearForm var(d);
  var.w1(i) = 1;
  return var;
}

int LinearForm::d() const {
  return d_;
}

QT &LinearForm::w0() {
  return w0_;
}

const QT &LinearForm::w0() const {
  return w0_;
}

std::vector<QT> &LinearForm::w1() {
  return w1_;
}

const std::vector<QT> &LinearForm::w1() const {
  return w1_;
}

QT &LinearForm::w1(int i) {
  return w1_[i];
}

const QT &LinearForm::w1(int i) const {
  return w1_[i];
}

LinearForm &LinearForm::operator+=(const LinearForm &other) {
  assert(d_ == other.d_);

  w0_ += other.w0_;
  for (int i = 0; i < d_; i++)
    w1_[i] += other.w1_[i];

  return *this;
}

LinearForm &LinearForm::operator-=(const LinearForm &other) {
  assert(d_ == other.d_);

  w0_ -= other.w0_;
  for (int i = 0; i < d_; i++)
    w1_[i] -= other.w1_[i];

  return *this;
}

LinearForm &LinearForm::operator*=(const QT &c) {
  w0_ *= c;
  for (int i = 0; i < d_; i++)
    w1_[i] *= c;

  return *this;
}

LinearForm &LinearForm::operator/=(const QT &c) {
  w0_ /= c;
  for (int i = 0; i < d_; i++)
    w1_[i] /= c;

  return *this;
}

LinearForm LinearForm::operator+(
    const LinearForm &other) const {
  LinearForm res(*this);
  res += other;
  return res;
}

LinearForm LinearForm::operator-(
    const LinearForm &other) const {
  LinearForm res(*this);
  res -= other;
  return res;
}

LinearForm LinearForm::operator*(const QT &c) const {
  LinearForm res(*this);
  res *= c;
  return res;
}

LinearForm LinearForm::operator/(const QT &c) const {
  LinearForm res(*this);
  res /= c;
  return res;
}

QuadraticForm LinearForm::operator*(const LinearForm &other) const {
  assert(d_ == other.d_);

  QuadraticForm res(d_);
  res.w0() = w0_ * other.w0_;
  for (int i = 0; i < d_; i++)
    res.w1(i) = other.w1_[i] * w0_ + w1_[i] * other.w0_;
  for (int i = 0; i < d_; i++)
    for (int j = 0; j <= i; j++)
      res.w2(i, j) = w1_[i] * other.w1_[j] + w1_[j] * other.w1_[i];
  return res;
}

LinearForm LinearForm::operator-() const {
  return (*this) * -1;
}

bool LinearForm::operator==(const LinearForm &other) const {
  return w0_ == other.w0_ && w1_ == other.w1_;
}

bool LinearForm::operator!=(const LinearForm &other) const {
  return w0_ != other.w0_ || w1_ != other.w1_;
}

LinearForm &LinearForm::normalize() {
  w0_.normalize();
  for (auto &v : w1_)
    v.normalize();
  return *this;
}

QT LinearForm::operator()(std::vector<QT> v) const {
  assert(int(v.size()) == d_);

  QT res = w0_;
  for (int i = 0; i < d_; i++)
    res += w1_[i] * v[i];

  return res;
}

LinearForm operator*(const QT &c, const LinearForm &f) {
  return f * c;
}

