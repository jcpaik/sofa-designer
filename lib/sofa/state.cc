#include "state.h"

#include <iostream>

#include "expect.h"
#include "branch_tree.h"
#include "qp.h"

SofaState::SofaState(SofaBranchTree &tree)
    : ctx(tree.ctx),
      tree(tree), 
      e_({0}), 
      conds_(ctx.default_constraints()),
      id_(0) {
  update_();
}

SofaState::SofaState(const SofaState &s)
    : ctx(s.ctx), 
      tree(s.tree),
      e_(s.e_), 
      conds_(s.conds_), 
      area_(s.area_), 
      vars_(s.vars_),
      id_(s.id_),
      area_result_(s.area_result_) {
}

bool SofaState::is_valid() const { 
  return bool(area_result_);
}

QT SofaState::area() { 
  expect(is_valid());
  update_();
  return area_; 
}

std::vector<QT> SofaState::vars() { 
  expect(is_valid());
  update_();
  return vars_; 
}

void SofaState::impose(SofaConstraintProbe cond) {
  if (is_valid()) {
    conds_.push_back(cond);
    // Update only when current solution becomes invalid
    if (!ctx.ineq(cond)(vars_))
      update_();
  }
}

void SofaState::impose(const SofaConstraints &conds) {
  if (is_valid()) {
    bool skip_update = true;
    for (const auto &cond : conds) {
      conds_.push_back(cond);
      if (skip_update && !ctx.ineq(cond)(vars_))
        skip_update = false;
    }
    // Update only when current solution becomes invalid
    if (!skip_update)
      update_();
  }
}

SofaState SofaState::split(SofaConstraintProbe ineq) {
  expect(is_valid());

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
  if (is_valid()) {
    e_ = e;
    auto narea = (ctx.area(e_))(vars_);
    expect(area_ >= narea);
    area_ = narea;
    if (area_ < QT(22195, 10000)) // Optimization
      update_();
  }
}

SofaAreaResult SofaState::is_compatible(const LinearInequality &extra_ineq) const {
  return is_compatible(std::vector<LinearInequality>{extra_ineq});
}

SofaAreaResult SofaState::is_compatible(
    const std::vector<LinearInequality> &extra_ineqs) const {
  // node already invalid by itself
  if (!is_valid())
    return area_result_;

  auto sol = sofa_area_qp(
      ctx.area(e_), ctx, conds_, extra_ineqs);
  return sol;
}

void SofaState::update_() {
  area_result_ = sofa_area_qp(ctx.area(e_), ctx, conds_);
  if (area_result_) {
    area_ = area_result_.optimality_proof().max_area;
    vars_ = area_result_.optimality_proof().maximizer;
    expect((ctx.area(e_))(vars_) == area_);
    expect(area_ > QT(22195, 10000));
  } else {
    // state turned from valid to invalid
    tree.invalid_states_.push_back(*this);
  }
}
