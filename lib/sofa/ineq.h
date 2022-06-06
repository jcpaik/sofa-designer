#pragma once

#include <vector>

#include <CGAL/enum.h>

#include "number.h"
#include "forms.h"

class LinearInequality {
  public:
    // Constructors
    LinearInequality();
    LinearInequality(const std::vector<NT> &a, const NT &b,
                     CGAL::Comparison_result r);
    LinearInequality(const std::vector<QT> &a, const QT &b,
                     CGAL::Comparison_result r);

    // Accessors
    int d() const;
    std::vector<NT> &a();
    const std::vector<NT> &a() const;
    NT &a(int i);
    const NT &a(int i) const;
    NT &b();
    const NT &b() const;
    CGAL::Comparison_result &r();
    const CGAL::Comparison_result &r() const;

    LinearInequality negate() const;

    // Computation
    bool operator()(const std::vector<QT> &v) const;

    // Serialization
    void read_from(std::istream &in);
    void write_to(std::ostream &out);

  private:
    int d_;
    std::vector<NT> a_;
    NT b_;
    CGAL::Comparison_result r_;
};

// Concise way of writing inequalities from linear forms
LinearInequality operator<=(const LinearForm &lhs, const LinearForm &rhs);
LinearInequality operator>=(const LinearForm &lhs, const LinearForm &rhs);

// --------------- inline functions ---------------

inline int LinearInequality::d() const {
  return d_;
}

inline std::vector<NT> &LinearInequality::a() {
  return a_;
}

inline const std::vector<NT> &LinearInequality::a() const {
  return a_;
}

inline NT &LinearInequality::a(int i) {
  return a_[i];
}

inline const NT &LinearInequality::a(int i) const {
  return a_[i];
}

inline NT &LinearInequality::b() {
  return b_;
}

inline const NT &LinearInequality::b() const {
  return b_;
}

inline CGAL::Comparison_result &LinearInequality::r() {
  return r_;
}

inline const CGAL::Comparison_result &LinearInequality::r() const {
  return r_;
}

inline LinearInequality LinearInequality::negate() const {
  if (r_ == CGAL::SMALLER)
    return LinearInequality(a_, b_, CGAL::LARGER);
  else
    return LinearInequality(a_, b_, CGAL::SMALLER);
}

inline LinearInequality operator<=(
    const LinearForm &lhs, 
    const LinearForm &rhs) {
  LinearForm d = lhs - rhs;
  return LinearInequality(d.w1(), -d.w0(), CGAL::SMALLER);
}

inline LinearInequality operator>=(
    const LinearForm &lhs,
    const LinearForm &rhs) {
  LinearForm d = lhs - rhs;
  return LinearInequality(d.w1(), -d.w0(), CGAL::LARGER);
} 
