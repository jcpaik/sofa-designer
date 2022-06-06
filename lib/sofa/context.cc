#include "context.h"

#include "cereal.h"

SofaContext::SofaContext(const char *file) {
  load(file, *this);
}

void SofaContext::initialize(const std::vector<Vector> &u_in) {

  n_ = int(u_in.size()) + 1;
  d_ = 2 * n_ - 1;

  // Initialize u_ and v_
  u_ = {{1, 0}};
  u_.insert(u_.end(), u_in.begin(), u_in.end());
  for (int i = 0; i < 3 * n_; i++) {
    // Add rotations of u
    u_.emplace_back(-u_[i].y, u_[i].x);
  }
  // Check validity of u_ and v_
  for (int i = 0; i <= n_; i++) {
    assert(u_[i].norm_squared() == 1);
    assert(u_[i].x >= 0 && u_[i].y >= 0);
    if (i > 0) {
      assert(u_[i - 1].x > u_[i].x);
      assert(u_[i - 1].y < u_[i].y);
    }
  }

  // Linear form constants;
  LinearForm zero(d_);
  LinearForm one = LinearForm::constant(d_, 1);

  // Inequalities start from index 1
  assert(int(ineqs_.size()) == 0);
  ineqs_.resize(1); // The 'null' 0'th inequality

  // Prepare to add default_ineqs_ to ineqs_
  default_ineqs_offset_ = int(ineqs_.size());

  // Initialize edge length variables
  int vidx = 0;
  s_.resize(2 * n_ + 1);

  // Set right part of convex hull
  LinearForm aysum = zero;
  for (int i = 1; i <= n_ - 1; i++) {
    s_[i] = LinearForm::variable(d_, vidx++);
    aysum += s_[i] * v(i).y;
  }
  s_[0] = one - aysum;
  // Add corresponding constraint
  default_constraints_.push_back(int(ineqs_.size()));
  ineqs_.push_back(s_[0] >= zero);

  s_[n_] = LinearForm::variable(d_, vidx++);

  // Set left part of convex hull
  LinearForm cysum = LinearForm(d_);
  for (int i = 1; i <= n_ - 1; i++) {
    s_[n_ + i] = LinearForm::variable(d_, vidx++);
    cysum += s_[n_ + i] * u(i).y;
  }
  s_[2 * n_] = one - cysum;
  // Add corresponding constraint
  default_constraints_.push_back(int(ineqs_.size()));
  ineqs_.push_back(s_[2 * n_] >= zero);

  // Check the number of primitive variables introduced
  assert(vidx == d_);

  // Precompute area
  A_.resize(2 * n_ + 2);
  A_[0] = LinearFormPoint(d_);
  for (int i = 0; i <= 2 * n_; i++)
    A_[i + 1] = A_[i] + s_[i] * v(i);

  assert(A_[n_].y == one);
  assert(A_[n_ + 1].y == one);
  assert(A_[2 * n_ + 1].y == zero);

  // Compute lines
  a_.resize(2 * n_ + 1);
  b_.resize(2 * n_ + 1);
  for (int i = 0; i <= 2 * n_; i++) {
    a_[i] = b_[i] = Line(u(i), A_[i]);
    b_[i].b -= one;
  }

  // Compute outer area
  outer_area_ = polygon_area(A_);

  // Precompute intersection points
  int sz = (2 * n_ - 1);
  sz = sz * sz;
  p_.resize(sz);
  for (int i = -(n_ - 1); i <= (n_ - 1); i++)
    for (int j = -(n_ - 1); j <= (n_ - 1); j++)
      if (i != j)
        p_[index_(i, j)] = intersection(line(i), line(j));

  // Compute point areas
  sz = (2 * n_ - 1);
  sz = sz * sz * sz;
  inner_area_.resize(sz);
  for (int i = -(n_ - 1); i <= (n_ - 1); i++)
    for (int j = -(n_ - 1); j <= (n_ - 1); j++)
      for (int k = -(n_ - 1); k <= (n_ - 1); k++)
        if (i != j && j != k) {
          const LinearFormPoint &p1 = p_[index_(i, j)];
          const LinearFormPoint &p2 = p_[index_(j, k)];
          inner_area_[index_(i, j, k)] = (p1.y * p2.x - p1.x * p2.y) / 2;
        }

  // Compute left_ inequalities
  // The point p(i, j) is on the left side of p(l - n_, l)
  left_ineqs_offset_ = int(ineqs_.size());
  for (int l = 1; l <= (n_ - 1); l++)
    for (int j = -(n_ - 1); j <= (n_ - 1); j++)
      for (int i = -(n_ - 1); i < j; i++) {
        if (i != l - n_ && i != l && j != l - n_ && j != l) {
          assert(is_left(i, j, l) == int(ineqs_.size()));
          ineqs_.push_back(p(i, j).x <= p(l - n_, l).x);
        } else {
          ineqs_.emplace_back();
        }
      }

  // Compute over_ inequalities
  // The point p(i, j) is over the line k
  over_ineqs_offset_ = int(ineqs_.size());
  for (int k = -(n_ - 1); k <= (n_ - 1); k++)
    for (int j = -(n_ - 1); j < k; j++)
      for (int i = -(n_ - 1); i < j; i++) {
        assert(is_over(i, j, k) == int(ineqs_.size()));
        const auto &ptr = p(i, j);
        const auto d = dot(line(k).a, ptr);
        ineqs_.push_back(d >= line(k).b);
      }

  extra_ineqs_offset_ = int(ineqs_.size());

  auto rev_ineqs = ineqs_;
  for (auto &v : rev_ineqs)
    v = v.negate();
  ineqs_.insert(ineqs_.begin(), rev_ineqs.rbegin(), rev_ineqs.rend() - 1);
  ineqs_zero_ = ineqs_.begin() + rev_ineqs.size() - 1;
}

QuadraticForm SofaContext::area(const std::vector<int> &pl) const {
  int m = int(pl.size());
  assert(m >= 2);
  assert(pl.front() == 0);
  assert(pl.back() == 0);
  for (int i = 1; i <= m - 2; i++) {
    assert(-(n_ - 1) <= pl[i] && pl[i] <= (n_ - 1));
    assert(pl[i] != pl[i - 1] && pl[i] != pl[i + 1]);
  }
  QuadraticForm area = outer_area_;
  for (int i = 1; i <= m - 2; i++) {
    area -= inner_area_[index_(pl[i - 1], pl[i], pl[i + 1])];
  }
  return area;
}

