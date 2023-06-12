#include "state.h"

#include <iostream>

#include "expect.h"
#include "branch_tree.h"
#include "qp.h"
#include "cereal.h"

SofaState::SofaState(SofaBranchTree &tree, int i)
    : ctx(tree.ctx),
      tree(tree), 
      is_valid_(true), 
      e_({0, i, i - ctx.n(), 0}), 
      conds_(ctx.default_constraints()),
      id_(0) {
  conds_.push_back(ctx.is_over(i, i - ctx.n(), 0));
  update_();
}

SofaState::SofaState(const SofaState &s)
    : ctx(s.ctx), 
      tree(s.tree),
      is_valid_(s.is_valid_),
      e_(s.e_), 
      conds_(s.conds_), 
      area_(s.area_), 
      vars_(s.vars_),
      id_(s.id_) {
}

SofaState::SofaState(SofaBranchTree &tree, const char *file) 
    : ctx(tree.ctx), tree(tree) {
  load(file, *this);
}

SofaState::SofaState(SofaBranchTree &tree, CerealReader &reader)
    : ctx(tree.ctx), tree(tree) {
  reader >> *this;
}

bool SofaState::is_valid() const { 
  return is_valid_;
}

QT SofaState::area() { 
  update_();
  return area_; 
}

std::vector<QT> SofaState::vars() { 
  update_();
  return vars_; 
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

SofaState SofaState::split(SofaConstraintProbe ineq) {
  expect(is_valid_);

  int parent_id = this->id_;
  int child_left_id = tree.new_state_id_();
  int child_right_id = tree.new_state_id_();

  SofaState other(*this);
  this->impose(ineq);
  this->id_ = child_left_id;
  other.impose(-ineq);
  other.id_ = child_right_id;

  tree.split_states_.emplace_back(
    parent_id, ineq, child_left_id, child_right_id);

  return other;
}

const std::vector<int> &SofaState::e() const { 
  return e_; 
}

int SofaState::e(int i) const {
  return e_[i];
}

const LinearFormPoint &SofaState::p(int i) const { 
  return ctx.p(e(i), e(i+1));
}

Vector SofaState::v(int i) const {
  return p(i)(vars_);
}

void SofaState::update_e(const std::vector<int> &e) {
  if (is_valid_) {
    e_ = e;
    auto narea = (ctx.area(e_))(vars_);
    expect(area_ >= narea);
    area_ = narea;
    if (area_ < QT(22195, 10000)) // Optimization
      update_();
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

  auto sol = sofa_area_qp(
      ctx.area(e_), ctx, conds_, extra_ineqs);
  expect(sol.status != CGAL::QP_UNBOUNDED);
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

void SofaState::update_() {
  if (!is_valid_)
    return;
  
  auto sol = sofa_area_qp(ctx.area(e_), ctx, conds_);
  expect(sol.status != CGAL::QP_UNBOUNDED);
  if (sol.status == CGAL::QP_INFEASIBLE) {
    is_valid_ = false;
  } else {
    // sol.status == CGAL::QP_OPTIMAL
    area_ = sol.value;
    vars_ = sol.variables;
    expect((ctx.area(e_))(vars_) == area_);
    // TODO: change constant
    if (area_ < QT(22195, 10000)) {
      is_valid_ = false;
    }
  }

  // state turned into invalid one
  if (!is_valid_) {
    tree.invalid_states_.push_back(*this);
  }
}
