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
    int d() const; // dimension
    std::vector<NT> &a();
    const std::vector<NT> &a() const;
    NT &a(int i);
    const NT &a(int i) const;
    NT &b();
    const NT &b() const;
    CGAL::Comparison_result &r();
    const CGAL::Comparison_result &r() const;

    // Multiplication factor
    // Divide by this scale to retrieve the original inequality in QP
    const QT &scale() const;
    // A value v such that the inequality is equivalent to v >= 0
    const LinearForm nonneg_value() const;

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

    QT scale_;
};

// Concise way of writing inequalities from linear forms
LinearInequality operator<=(const LinearForm &lhs, const LinearForm &rhs);
LinearInequality operator>=(const LinearForm &lhs, const LinearForm &rhs);
LinearInequality operator<=(const LinearForm &lhs, const QT &rhs);
LinearInequality operator>=(const LinearForm &lhs, const QT &rhs);
