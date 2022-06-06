#pragma once

#include <cassert>
#include <algorithm>
#include <vector>
#include <tuple>

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
    // Convex hull edge length (0 <= i <= 2*n)
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
    const SofaConstraints &default_constraints() const;

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
    std::vector<LinearInequality> ineqs_;
    std::vector<LinearInequality>::const_iterator ineqs_zero_;
    int default_ineqs_offset_;
    int left_ineqs_offset_;
    int over_ineqs_offset_;
    int extra_ineqs_offset_;

    int index_(int i, int j) const;
    int index_(int i, int j, int k) const;
};

// -------------------- inlines ----------------------

inline SofaContext::SofaContext(const std::vector<Vector> &u) {
  initialize(u);
}

inline int SofaContext::n() const {
  return n_;
}

inline int SofaContext::d() const {
  return d_;
}

inline const LinearForm &SofaContext::s(int i) const {
  return s_[i];
}

inline const LinearFormPoint &SofaContext::A(int i) const {
  return A_[i];
}

inline const LinearFormPoint &SofaContext::C(int i) const {
  return A_[n_ + 1 + i];
}

inline const Line &SofaContext::a(int i) const {
  return a_[i];
}

inline const Line &SofaContext::b(int i) const {
  return b_[i];
}

inline const Line &SofaContext::c(int i) const {
  return a_[n_ + i];
}

inline const Line &SofaContext::d(int i) const {
  return b_[n_ + i];
}

inline const Line &SofaContext::line(int i) const {
  return d(i);
}

inline const LinearFormPoint &SofaContext::p(int i, int j) const {
  return p_[index_(i, j)];
}

inline const LinearFormPoint SofaContext::x(int i) const {
  // Linear form constants
  LinearForm zero(d_);
  LinearForm one = LinearForm::constant(d_, 1);
  if (i == 0)
    return {A(0).x - one, zero};
  else if (i == n_)
    return {C(n_).x + one, zero};
  else
    return p(i - n_, i);
}

inline const SofaConstraints 
&SofaContext::default_constraints() const {
  return default_constraints_;
}

inline SofaConstraintProbe SofaContext::is_left(int i, int j, int l) const {
  if (i > j)
    std::swap(i, j);
  assert(-(n_ - 1) <= i && i < j && j <= (n_ - 1));
  assert(1 <= l && l <= (n_ - 1));
  assert(i != l - n_ && i != l && j != l - n_ && j != l);
  i += (n_ - 1);
  j += (n_ - 1);
  // 2n_ - 1 C 2    
  l -= 1;
  return left_ineqs_offset_ + 
    (2 * n_ - 1) * (n_ - 1) * l + 
    j * (j - 1) / 2 + 
    i;
}

inline SofaConstraintProbe SofaContext::is_right(int i, int j, int l) const {
  return -is_left(i, j, l);
}

inline SofaConstraintProbe SofaContext::is_over(int i, int j, int l) const {
  assert(i != j && j != l && l != i);
  if (i > j)
    std::swap(i, j);
  bool flip = (i < l && l < j);
  if (j > l)
    std::swap(j, l);
  if (i > j)
    std::swap(i, j);
  i += (n_ - 1);
  j += (n_ - 1);
  l += (n_ - 1);
  int probe = over_ineqs_offset_ + 
    l * (l - 1) / 2 * (l - 2) / 3 + 
    j * (j - 1) / 2 +
    i;
  return flip ? -probe : probe;
}

inline SofaConstraintProbe SofaContext::is_under(int i, int j, int l) const {
  return -is_over(i, j, l);
}

inline const LinearInequality &SofaContext::ineq(SofaConstraintProbe i) const {
  return ineqs_zero_[i];
}

inline int SofaContext::extra_ineqs_offset() const {
  return extra_ineqs_offset_;
}

inline SofaContext::ProbeToA SofaContext::probe_to_a(int dim) const {
  return SofaContext::ProbeToA {dim, extra_ineqs_offset_, ineqs_zero_};
}

inline SofaContext::ProbeToB SofaContext::probe_to_b() const {
  return SofaContext::ProbeToB {extra_ineqs_offset_, ineqs_zero_};
}

inline SofaContext::ProbeToR SofaContext::probe_to_r() const {
  return SofaContext::ProbeToR {extra_ineqs_offset_, ineqs_zero_};
}

inline const Vector &SofaContext::u(int i) const {
  return u_[(i % (4 * n_) + 4 * n_) % (4 * n_)];
}

inline const Vector &SofaContext::v(int i) const {
  return u_[(i % (4 * n_) + 5 * n_) % (4 * n_)];
}

inline int SofaContext::index_(int i, int j) const {
  assert(i >= -(n_ - 1) && i <= n_ - 1);
  assert(j >= -(n_ - 1) && j <= n_ - 1);
  i += (n_ - 1);
  j += (n_ - 1);
  return i * (2 * n_ - 1) + j;
}

inline int SofaContext::index_(int i, int j, int k) const {
  assert(i >= -(n_ - 1) && i <= n_ - 1);
  assert(j >= -(n_ - 1) && j <= n_ - 1);
  assert(k >= -(n_ - 1) && k <= n_ - 1);
  i += (n_ - 1);
  j += (n_ - 1);
  k += (n_ - 1);
  return i * (2 * n_ - 1) * (2 * n_ - 1) + j * (2 * n_ - 1) + k;
}
