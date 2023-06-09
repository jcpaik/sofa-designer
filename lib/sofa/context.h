#pragma once

#include <cassert>
#include <algorithm>
#include <string>
#include <tuple>
#include <vector>

#include <json/json.h>

#include "number.h"
#include "forms.h"
#include "ineq.h"
#include "geom.h"

using SofaConstraintProbe = int;
using SofaConstraints = std::vector<SofaConstraintProbe>;

class CerealReader;
class CerealWriter;

class SofaContext {
  public:
    SofaContext() = delete;
    // u: The list of nontrivial unit vectors on the 1st quadrant
    //    to be used. This corresponds to the partition \Gamma.
    //    The vectors corresponding to (0, 1) and (1, 0) are skipped,
    //    so n = u.size() + 1.
    SofaContext(const std::vector<Vector> &u);
    // Allows loading from file
    explicit SofaContext(const char *file);

    // Does the same thing as constructor, replacing the original context
    void initialize(const std::vector<Vector> &u);

    // Size of partition
    int n() const;
    // The number of variables d = 2n-1 involved
    // Two out of 2n+1 sides are eliminated
    int d() const;
    // Convex hull support function (0 <= i <= 2*n)
    const LinearForm &s(int i) const;
    // Vertices (right) (0 <= i <= 2*n+1)
    const LinearFormPoint &A(int i) const;
    // Vertices (left) (-n-1 <= i <= n)
    const LinearFormPoint &C(int i) const;
    // Lines (0 <= i <= 2*n)
    const Line &a(int i) const;
    const Line &b(int i) const;
    // (-n <= i <= n)
    const Line &c(int i) const;
    const Line &d(int i) const;
    // Alias for d
    const Line &line(int i) const;

    // The intersection of line(i) and line(j) memoized 
    // -(n-1) <= i, j <= n-1, i != j
    const LinearFormPoint &p(int i, int j) const;
    // The intersection of line(i - n) and line(i)
    // -n <= i <= n
    const LinearFormPoint x(int i) const;
    // Default constraints for sofa normality
    SofaConstraints default_constraints() const;

    SofaConstraintProbe is_left(int i, int j, int l) const;
    SofaConstraintProbe is_right(int i, int j, int l) const;
    SofaConstraintProbe is_over(int i, int j, int l) const;
    SofaConstraintProbe is_under(int i, int j, int l) const;
  
    const LinearInequality &ineq(SofaConstraintProbe i) const; 
    int extra_ineqs_offset() const;

    // minor TODO: hide implementation of ProbeTo functors
    // According to Boost documentation, the functors should be
    // assignable and copy constructable.
    struct ProbeToA {
        int dim;
        int extra_ineqs_offset;
        std::vector<LinearInequality>::const_iterator ineqs_zero, extra_ineqs;

        // Returns the coefficient of `dim`th coordinate
        // of the `i`th inequality
        const NT &operator()(int i) const {
          if (i < extra_ineqs_offset) {
            return ineqs_zero[i].a(dim);
          } else {
            return extra_ineqs[i - extra_ineqs_offset].a(dim);
          }
        }
    };

    struct ProbeToB {
        int extra_ineqs_offset;
        std::vector<LinearInequality>::const_iterator ineqs_zero, extra_ineqs;

        const NT &operator()(int i) const {
          if (i < extra_ineqs_offset) {
            return ineqs_zero[i].b();
          } else {
            return extra_ineqs[i - extra_ineqs_offset].b();
          }
        }
    };

    struct ProbeToR {
        int extra_ineqs_offset;
        std::vector<LinearInequality>::const_iterator ineqs_zero, extra_ineqs;

        CGAL::Comparison_result operator()(int i) const {
          if (i < extra_ineqs_offset) {
            return ineqs_zero[i].r();
          } else {
            return extra_ineqs[i - extra_ineqs_offset].r();
          }
        }
    };

    ProbeToA probe_to_a(int dim) const;
    ProbeToB probe_to_b() const;
    ProbeToR probe_to_r() const;

    // Unit vectors u and v.
    // The definition of u and v extends to any integer i.
    const Vector &u(int i) const;
    const Vector &v(int i) const;

    // Area function for a specified shape
    QuadraticForm area(const std::vector<int> &polyline) const;

    friend CerealWriter &operator<<(CerealWriter &out, const SofaContext &v);
    friend CerealReader &operator>>(CerealReader &in, SofaContext &v);

    Json::Value split_values() const;

  private:
    int n_, d_;
    std::vector<Vector> u_;

    std::vector<Line> a_, b_;

    std::vector<LinearForm> s_;
    std::vector<LinearFormPoint> A_;
    QuadraticForm outer_area_;
    std::vector<LinearFormPoint> p_;
    std::vector<QuadraticForm> inner_area_;

    std::vector<SofaConstraintProbe> default_constraints_;
    // !i9, !i8, !i7, ..., !i1, 0, i1, i2, ..., i9
    std::vector<LinearInequality> ineqs_;
    // points to ineq zero
    std::vector<LinearInequality>::const_iterator ineqs_zero_;
    std::vector<std::string> ineq_names_;
    // Inequalities appear in this order in ineqs_
    SofaConstraintProbe default_ineqs_offset_;
    SofaConstraintProbe left_ineqs_offset_;
    SofaConstraintProbe over_ineqs_offset_;
    SofaConstraintProbe extra_ineqs_offset_;

    void add_ineq_(const LinearInequality &ineq, const std::string &name);

    int index_(int i, int j) const;
    int index_(int i, int j, int k) const;
};
