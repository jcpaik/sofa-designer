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

SofaAreaResult sofa_area_qp(
    const QuadraticForm &q, 
    const SofaContext &ctx,
    SofaConstraints ineqs,
    const std::vector<LinearInequality> &extra_ineqs) {
  expect(q.d() == ctx.d());
  expect(is_negative_semidefinite(q));

  int n = q.d();
  int m = ineqs.size();

  // The CGAL solver accepts integer types
  // so we need to multiply the whole program with common denominator

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
      m + num_extra_ineqs,
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

  auto max_area = (-sol.objective_value() / d).normalize();

  expect(sol.status() != CGAL::QP_UNBOUNDED);
  if (sol.is_infeasible()) {
    // identify infeasibility proof and return it
    std::map<SofaConstraintProbe, QT> lambdas;
    std::map<int, QT> lambdas_extra;
    for (int i = 0; i < m + num_extra_ineqs; i++) {
      QT lambda = *(sol.infeasibility_certificate_begin() + i);
      if (lambda == 0)
        continue;
      
      const LinearInequality& ineq = (i < m) ?
          ctx.ineq(ineqs[i]) : extra_ineqs[i - m];
      expect(
          (ineq.r() == CGAL::SMALLER && lambda <= 0) ||
          (ineq.r() == CGAL::LARGER && lambda >= 0));
      lambda *= ineq.scale();
      if (lambda < 0)
        lambda = -lambda;
      
      if (i < m)
        lambdas[i] = lambda;
      else
        lambdas_extra[i - m] = lambda;
    }

    // check contradiction
    LinearForm x(n);
    for (auto const& [id, lambda] : lambdas) {
      expect(lambda > 0);
      x += ctx.ineq(id).nonneg_value() * lambda;
    }
    for (auto const& [id, lambda] : lambdas_extra) {
      expect(lambda > 0);
      x += extra_ineqs[id].nonneg_value() * lambda;
    }
    // we added nonnegative values, but the result is strictly negative
    expect(x.w0() < 0);
    for (const auto &val : x.w1())
      expect(val <= 0);
    
    return {SofaAreaInvalidityProof{lambdas, lambdas_extra}};
  } else {
    // sol.status == CGAL::QP_OPTIMAL
    return { 
      std::vector<QT>(sol.variable_values_begin(), sol.variable_values_end()), 
      max_area 
    };
  }
}
