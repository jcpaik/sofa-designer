#include "qp.h"

#include <algorithm>

#include "expect.h"

bool is_negative_semidefinite(const QuadraticForm &q) {
  return is_negative_semidefinite(q.w2());
}

bool is_negative_semidefinite(const std::vector< std::vector<QT> > &mat) {
  int n = int(mat.size());
  std::vector<QT> d(n);
  auto a = mat;
  std::vector<int> perm(n);
  for (int i = 0; i < n; i++)
    perm[i] = i;
  for (int k = 0; k < n; k++) {
    // set up d[k]
    // or determine that we are already 'done'
    int p = k;
    while (p < n && a[p][p] == 0)
      p++;
    if (p == n) { // if all diagonals are zero
      // routine for checking nonzeroness
      for (int i = k; i < n; i++)
        for (int j = k; j <= i; j++)
          if (a[i][j] != 0)
            return false;
      break;
    } else if (a[p][p] > 0) { // if positive diagonal found
      return false;
    } else if (p > k) { // if diagonal needs re-arrangement
      std::swap(perm[k], perm[p]);
      for (int i = 0; i < k; i++)
          std::swap(a[k][i], a[p][i]);
      for (int i = k + 1; i < p; i++)
          std::swap(a[i][k], a[p][i]);
      for (int i = p + 1; i < n; i++)
          std::swap(a[i][k], a[i][p]);
      std::swap(a[k][k], a[p][p]);
    }

    expect(a[k][k] < 0);
    d[k] = a[k][k];
    for (int i = k; i < n; i++)
      a[i][k] /= d[k];
    for (int i = k + 1; i < n; i++)
      for (int j = k + 1; j <= i; j++)
        a[i][j] -= a[i][k] * a[j][k] * d[k];
  }
  for (int i = 0; i < n; i++)
    expect(d[i] <= 0);

  // `a` is a certificate with permutation vector perm
  // row [i] of mat is in row[perm[i]] of a
  for (int i = 0; i < n; i++) {
    // int pi = perm[i];
    for (int j = 0; j <= i; j++) {
      // int pj = perm[j];
      // checks mat[i][j]
      QT tot = 0;
      for (int k = 0; k <= i && k <= j; k++)
        tot += a[i][k] * a[j][k] * d[k];
      expect((perm[i] >= perm[j] ? 
            mat[perm[i]][perm[j]] : 
            mat[perm[j]][perm[i]]) == tot);
    }
  }

  return true;
}

NonnegativeQPSolution nonnegative_maximize_quadratic_form(
    const QuadraticForm &q, 
    const SofaContext &ctx,
    SofaConstraints ineqs,
    const std::vector<LinearInequality> &extra_ineqs) {
  expect(q.d() == ctx.d());
  expect(is_negative_semidefinite(q));

  int n = q.d();
  int m = ineqs.size();

  // Find the least common multiple of denominators
  NT d = q.w0().denominator();
  for (int j = 0; j < n; j++) {
    NT v = q.w1(j).denominator();
    d *= v / CGAL::gcd(d, v);
  }
  for (int i = 0; i < n; i++) {
    for (int j = 0; j <= i; j++) {
      NT v = q.w2(i, j).denominator();
      d *= v / CGAL::gcd(d, v);
    }
  }

  // c0
  NT c0(- q.w0().numerator() * (d / q.w0().denominator()));

  // c
  std::vector<NT> c(n);
  for (int j = 0; j < n; j++)
    c[j] = - q.w1(j).numerator() * (d / q.w1(j).denominator());

  // d
  std::vector< std::vector<NT> > d_mat(n);
  std::vector< std::vector<NT>::const_iterator > d_iters(n);
  for (int i = 0; i < n; i++) {
    auto &row = d_mat[i];
    row.resize(i + 1);
    for (int j = 0; j <= i; j++) {
      const auto &num = q.w2(i, j);
      row[j] = - num.numerator() * (d / num.denominator());
    }
    d_iters[i] = row.begin();
  }

  // TODO: This is too complex
  // Inject extra inequalities 
  // and transform whole indices of inequalities
  // to coefficients of inequalities
  int extra_probe = ctx.extra_ineqs_offset();
  int num_extra_ineqs = int(extra_ineqs.size());
  for (int i = 0; i < num_extra_ineqs; i++) {
    ineqs.push_back(extra_probe);
    extra_probe++;
  }
  m += num_extra_ineqs;

  using AIter = boost::transform_iterator<
    SofaContext::ProbeToA, SofaConstraints::const_iterator>;
  std::vector<AIter> a_iters;
  for (int i = 0; i < n; i++) {
    auto p2a = ctx.probe_to_a(i);
    p2a.extra_ineqs = extra_ineqs.begin();
    a_iters.emplace_back(ineqs.begin(), p2a);
  }
  auto p2b = ctx.probe_to_b();
  p2b.extra_ineqs = extra_ineqs.begin();
  auto b_iter = boost::make_transform_iterator(ineqs.begin(), p2b);
  auto p2r = ctx.probe_to_r();
  p2r.extra_ineqs = extra_ineqs.begin();
  auto r_iter = boost::make_transform_iterator(ineqs.begin(), p2r);

  auto qp = CGAL::make_nonnegative_quadratic_program_from_iterators(
      n,
      m,
      a_iters.begin(),
      b_iter,
      r_iter,
      d_iters.begin(),
      c.begin(),
      c0);

  CGAL::Quadratic_program_options ops;
  // prevents solver from hitting infinite loop
  ops.set_pricing_strategy(CGAL::QP_BLAND);
  auto sol = CGAL::solve_nonnegative_quadratic_program(qp, NT(), ops);
  expect(sol.solves_quadratic_program(qp));

  auto value = -sol.objective_value() / d;

  return { 
    sol.status(), 
    std::vector<QT>(sol.variable_values_begin(), sol.variable_values_end()), 
    value.normalize() 
  };
}
