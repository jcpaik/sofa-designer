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
