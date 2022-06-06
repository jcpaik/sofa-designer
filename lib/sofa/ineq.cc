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
