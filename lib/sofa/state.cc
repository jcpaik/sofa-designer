#include "state.h"

#include <cassert>
#include <iostream>

#include "qp.h"
#include "cereal.h"

SofaState::SofaState(const SofaContext &ctx, const char *file) : ctx(ctx) {
  load(file, *this);
}

SofaState::SofaState(const SofaContext &ctx, CerealReader &reader) : ctx(ctx) {
  reader >> *this;
}

void SofaState::update_() {
  if (is_valid_) {
    auto sol = nonnegative_maximize_quadratic_form(ctx.area(e_), ctx, conds_);
    assert(sol.status != CGAL::QP_UNBOUNDED);
    if (sol.status == CGAL::QP_INFEASIBLE) {
      is_valid_ = false;
      return;
    }
    // sol.status == CGAL::QP_OPTIMAL
    area_ = sol.value;
    vars_ = sol.variables;
    assert((ctx.area(e_))(vars_) == area_);
    // TODO: change constant
    if (area_ < QT(22195, 10000)) {
      is_valid_ = false;
      return;
    }
  }
}

bool SofaState::is_compatible(const LinearInequality &extra_ineq) const {
  std::vector<LinearInequality> extra_ineqs(1, extra_ineq);
  bool res = is_compatible(extra_ineqs);
  return res;
}

bool SofaState::is_compatible(
    const std::vector<LinearInequality> &extra_ineqs) const {
  if (!is_valid_)
    return false; // If not valid, it's incompatible with any condition

  auto sol = nonnegative_maximize_quadratic_form(
      ctx.area(e_), ctx, conds_, extra_ineqs);
  assert(sol.status != CGAL::QP_UNBOUNDED);
  if (sol.status == CGAL::QP_INFEASIBLE) {
    return false;
  }
  // sol.status == CGAL::QP_OPTIMAL
  // TODO: change constant
  if (sol.value < QT(22195, 10000)) {
    return false;
  }
  return true;
}

void SofaState::impose(SofaConstraintProbe cond) {
  if (is_valid_) {
    conds_.push_back(cond);
    if (!ctx.ineq(cond)(vars_)) // Optimization logic
      update_();
  }
}

void SofaState::impose(const SofaConstraints &conds) {
  if (is_valid_) {
    bool skip_update = true;
    for (const auto &cond : conds) {
      conds_.push_back(cond);
      if (skip_update && !ctx.ineq(cond)(vars_))
        skip_update = false;
    }
    if (!skip_update)
      update_();
  }
}
