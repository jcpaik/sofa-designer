#include "lform.h"

#include "qform.h"

LinearForm::LinearForm() = default;

LinearForm::LinearForm(int d) : d_(d), w0_(), w1_(d) {
}

LinearForm::LinearForm(const QT &w0, const std::vector<QT> &w1)
    : d_(int(w1.size())), w0_(w0), w1_(w1) {
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
