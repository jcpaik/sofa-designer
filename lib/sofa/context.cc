#include "context.h"

#include <string>

#include "expect.h"
#include "cereal.h"

SofaContext::SofaContext(const std::vector<Vector> &u) {
  initialize(u);
}

SofaContext::SofaContext(const Json::Value &angles) {
  std::vector<Vector> gamma;

  for (int i = 0; i < angles.size(); i++) {
    const auto &angle = angles[i];
    gamma.emplace_back(
      qt_from_json(angle["cos"]),
      qt_from_json(angle["sin"]));
  }

  initialize(gamma);
} 

SofaContext::SofaContext(const char *file) {
  load(file, *this);
}

void SofaContext::add_ineq_(const LinearInequality &ineq, const std::string &name) {
    ineqs_.push_back(ineq);
    ineq_names_.push_back(name);
}

void SofaContext::initialize(const std::vector<Vector> &u_in) {
  // Inequalities start from index 1
  expect(int(ineqs_.size()) == 0);
  // The 'null' 0'th inequality
  ineqs_.resize(1);
  ineq_names_.resize(1);

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
    expect(u_[i].norm_squared() == 1);
    expect(u_[i].x >= 0 && u_[i].y >= 0);
    if (i > 0) {
      expect(u_[i - 1].x > u_[i].x);
      expect(u_[i - 1].y < u_[i].y);
    }
  }

  // Linear form constants;
  LinearForm zero(d_);
  LinearForm one = LinearForm::constant(d_, 1);

  // Initialize support value variables
  int vidx = 0;
  s_.resize(2 * n_ + 1);

  // Set support values of cap
  s_[0] = zero;
  for (int i = 1; i <= n_ - 1; i++) {
    s_[i] = LinearForm::variable(d_, vidx++);
  }
  s_[n_] = one;
  for (int i = n_ + 1; i <= 2 * n_; i++) {
    s_[i] = LinearForm::variable(d_, vidx++);
  }

  // Check the number of primitive variables introduced
  expect(vidx == d_);

  // Set lines
  a_.resize(2 * n_ + 1);
  b_.resize(2 * n_ + 1);
  for (int i = 0; i <= 2 * n_; i++) {
    a_[i] = b_[i] = Line(u(i), s_[i]);
    b_[i].b -= one;
  }

  // Compute intersections
  A_.resize(2 * n_ + 2);
  A_[0] = LinearFormPoint(d_);
  for (int i = 1; i <= 2 * n_; i++)
    A_[i] = intersection(a_[i - 1], a_[i]);
  A_[2 * n_ + 1] = LinearFormPoint(-s_[2 * n_], zero);

  expect(A_[n_].y == one);
  expect(A_[n_ + 1].y == one);
  expect(A_[2 * n_ + 1].y == zero);
  expect(A_[1].x == A_[0].x);
  expect(A_[2 * n_].x == A_[2 * n_ + 1].x);

  // Prepare to add default_ineqs_ to ineqs_
  default_ineqs_offset_ = int(ineqs_.size());
  // Add boundedness constraint
  add_ineq_(10 * one - s_[2 * n_] >= 0, "b");
  // Add convexity constraints
  for (int i = 0; i <= 2 * n_; i++)
    add_ineq_(
      dot(A_[i + 1] - A_[i], v(i)) >= zero, 
      std::string("s ") + std::to_string(i));

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
        expect(is_left(i, j, l) == int(ineqs_.size()));
        add_ineq_(
          p(l - n_, l).x - p(i, j).x >= 0,
          std::string("l ") + std::to_string(i) + " " +
          std::to_string(j) + " " + std::to_string(l));
      }

  // Compute over_ inequalities
  // The point p(i, j) is over the line k
  over_ineqs_offset_ = int(ineqs_.size());
  for (int k = -(n_ - 1); k <= (n_ - 1); k++)
    for (int j = -(n_ - 1); j < k; j++)
      for (int i = -(n_ - 1); i < j; i++) {
        expect(is_over(i, j, k) == int(ineqs_.size()));
        const auto &ptr = p(i, j);
        const auto d = dot(line(k).a, ptr);
        add_ineq_(d - line(k).b >= 0,
          std::string("o ") + std::to_string(i) + " " +
          std::to_string(j) + " " + std::to_string(k));
      }

  extra_ineqs_offset_ = int(ineqs_.size());

  auto rev_ineqs = ineqs_;
  for (auto &v : rev_ineqs)
    v = v.negate();
  ineqs_.insert(ineqs_.begin(), rev_ineqs.rbegin(), rev_ineqs.rend() - 1);
  ineqs_zero_ = ineqs_.begin() + rev_ineqs.size() - 1;
}

int SofaContext::n() const {
  return n_;
}

int SofaContext::d() const {
  return d_;
}

const LinearForm &SofaContext::s(int i) const {
  return s_[i];
}

const LinearFormPoint &SofaContext::A(int i) const {
  return A_[i];
}

const LinearFormPoint &SofaContext::C(int i) const {
  return A_[n_ + 1 + i];
}

const Line &SofaContext::a(int i) const {
  return a_[i];
}

const Line &SofaContext::b(int i) const {
  return b_[i];
}

const Line &SofaContext::c(int i) const {
  return a_[n_ + i];
}

const Line &SofaContext::d(int i) const {
  return b_[n_ + i];
}

const Line &SofaContext::line(int i) const {
  return d(i);
}

const LinearFormPoint &SofaContext::p(int i, int j) const {
  return p_[index_(i, j)];
}

const LinearFormPoint SofaContext::x(int i) const {
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

SofaConstraints SofaContext::default_constraints() const {
  SofaConstraints c;
  for (auto i = default_ineqs_offset_;
    i < left_ineqs_offset_;
    i++)
    c.push_back(i);
  return c;
}

SofaConstraintProbe SofaContext::is_left(int i, int j, int l) const {
  if (i > j)
    std::swap(i, j);
  expect(-(n_ - 1) <= i && i < j && j <= (n_ - 1));
  expect(1 <= l && l <= (n_ - 1));
  i += (n_ - 1);
  j += (n_ - 1);
  // 2n_ - 1 C 2    
  l -= 1;
  return left_ineqs_offset_ + 
    (2 * n_ - 1) * (n_ - 1) * l + 
    j * (j - 1) / 2 + 
    i;
}

SofaConstraintProbe SofaContext::is_right(int i, int j, int l) const {
  return -is_left(i, j, l);
}

SofaConstraintProbe SofaContext::is_over(int i, int j, int l) const {
  expect(i != j && j != l && l != i);
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

SofaConstraintProbe SofaContext::is_under(int i, int j, int l) const {
  return -is_over(i, j, l);
}

const LinearInequality &SofaContext::ineq(SofaConstraintProbe i) const {
  return ineqs_zero_[i];
}

// TODO: remove extra_ineqs_offset_
int SofaContext::extra_ineqs_offset() const {
  return extra_ineqs_offset_;
}

SofaContext::ProbeToA SofaContext::probe_to_a(int dim) const {
  return SofaContext::ProbeToA {dim, extra_ineqs_offset_, ineqs_zero_};
}

SofaContext::ProbeToB SofaContext::probe_to_b() const {
  return SofaContext::ProbeToB {extra_ineqs_offset_, ineqs_zero_};
}

SofaContext::ProbeToR SofaContext::probe_to_r() const {
  return SofaContext::ProbeToR {extra_ineqs_offset_, ineqs_zero_};
}

const Vector &SofaContext::u(int i) const {
  return u_[(i % (4 * n_) + 4 * n_) % (4 * n_)];
}

const Vector &SofaContext::v(int i) const {
  return u_[(i % (4 * n_) + 5 * n_) % (4 * n_)];
}

QuadraticForm SofaContext::area(const std::vector<int> &pl) const {
  int m = int(pl.size());
  expect(m >= 1);
  expect(pl.front() == 0);
  expect(pl.back() == 0);
  for (int i = 1; i <= m - 2; i++) {
    expect(-(n_ - 1) <= pl[i] && pl[i] <= (n_ - 1));
    expect(pl[i] != pl[i - 1] && pl[i] != pl[i + 1]);
  }
  QuadraticForm area = outer_area_;
  for (int i = 1; i <= m - 2; i++) {
    area -= inner_area_[index_(pl[i - 1], pl[i], pl[i + 1])];
  }
  return area;
}

Json::Value SofaContext::split_values() const {
  Json::Value values(Json::arrayValue);
  values.append(Json::Value::null);

  for (int id = 1; id < extra_ineqs_offset_; id++) {
    const auto &cur_ineq = ineq(id);
    const LinearForm lform = cur_ineq.nonneg_value();
    const auto &ineq_name = ineq_names_[id];
    if (ineq_name == "invalid")
      continue;
    
    Json::Value value(Json::objectValue);
    value["name"] = ineq_name;
    value["a"] = to_json(lform.w1());
    value["b"] = to_json(lform.w0());
    values.append(value);
  }
  return values;
}

int SofaContext::index_(int i, int j) const {
  expect(i >= -(n_ - 1) && i <= n_ - 1);
  expect(j >= -(n_ - 1) && j <= n_ - 1);
  i += (n_ - 1);
  j += (n_ - 1);
  return i * (2 * n_ - 1) + j;
}

int SofaContext::index_(int i, int j, int k) const {
  expect(i >= -(n_ - 1) && i <= n_ - 1);
  expect(j >= -(n_ - 1) && j <= n_ - 1);
  expect(k >= -(n_ - 1) && k <= n_ - 1);
  i += (n_ - 1);
  j += (n_ - 1);
  k += (n_ - 1);
  return i * (2 * n_ - 1) * (2 * n_ - 1) + j * (2 * n_ - 1) + k;
}
