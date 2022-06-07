#include "ineq.h"

#include <cassert>

LinearInequality::LinearInequality() = default;

LinearInequality::LinearInequality(const std::vector<NT> &a, const NT &b, 
                               CGAL::Comparison_result r)
    : d_(a.size()), a_(a), b_(b), r_(r) {
  assert(r != CGAL::EQUAL);
  NT g = b_;
  for (const auto &v : a_)
    g = CGAL::gcd(g, v);

  if (g != 0) {
    b_ /= g;
    for (auto &v : a_)
      v /= g;
  }
}

LinearInequality::LinearInequality(const std::vector<QT> &a, const QT &b,
                               CGAL::Comparison_result r)
    : d_(a.size()), a_(a.size()), b_(), r_(r) {
  assert(r != CGAL::EQUAL);
  NT d = b.denominator();
  for (int i = 0; i < d_; i++) {
    NT ad = a[i].denominator();
    d *= ad / CGAL::gcd(d, ad);
  }

  for (int i = 0; i < d_; i++)
    a_[i] = a[i].numerator() * (d / a[i].denominator());
  b_ = b.numerator() * (d / b.denominator());
}

int LinearInequality::d() const {
  return d_;
}

std::vector<NT> &LinearInequality::a() {
  return a_;
}

const std::vector<NT> &LinearInequality::a() const {
  return a_;
}

NT &LinearInequality::a(int i) {
  return a_[i];
}

const NT &LinearInequality::a(int i) const {
  return a_[i];
}

NT &LinearInequality::b() {
  return b_;
}

const NT &LinearInequality::b() const {
  return b_;
}

CGAL::Comparison_result &LinearInequality::r() {
  return r_;
}

const CGAL::Comparison_result &LinearInequality::r() const {
  return r_;
}

LinearInequality LinearInequality::negate() const {
  if (r_ == CGAL::SMALLER)
    return LinearInequality(a_, b_, CGAL::LARGER);
  else
    return LinearInequality(a_, b_, CGAL::SMALLER);
}

bool LinearInequality::operator()(const std::vector<QT> &v) const {
  assert(int(v.size()) == d_);

  QT asum;
  for (int i = 0; i < d_; i++)
    asum += a_[i] * v[i];

  if (r_ == CGAL::SMALLER)
    return asum <= b_;
  else
    return asum >= b_;
}

LinearInequality operator<=(
    const LinearForm &lhs, 
    const LinearForm &rhs) {
  LinearForm d = lhs - rhs;
  return LinearInequality(d.w1(), -d.w0(), CGAL::SMALLER);
}

LinearInequality operator>=(
    const LinearForm &lhs,
    const LinearForm &rhs) {
  LinearForm d = lhs - rhs;
  return LinearInequality(d.w1(), -d.w0(), CGAL::LARGER);
} 
