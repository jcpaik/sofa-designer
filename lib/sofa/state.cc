#include "state.h"

#include <iostream>

#include "json.h"
#include "expect.h"
#include "branch_tree.h"
#include "qp.h"
#include "cereal.h"

SofaState::SofaState(SofaBranchTree &tree)
    : ctx(tree.ctx),
      tree(tree), 
      id_(0),
      e_({0}), 
      conds_(ctx.default_constraints()),
      is_frozen_(false) {
  update_();
}

SofaState::SofaState(const SofaState &s)
    : ctx(s.ctx), 
      tree(s.tree),
      id_(s.id_),
      is_valid_(s.is_valid_),
      e_(s.e_), 
      conds_(s.conds_), 
      area_(s.area_), 
      vars_(s.vars_),
      is_frozen_(s.is_frozen_),
      area_result_(s.area_result_) {
}

SofaState::SofaState(SofaBranchTree &tree, const char *file) 
    : ctx(tree.ctx), tree(tree), is_frozen_(true) {
  load(file, *this);
}

SofaState::SofaState(SofaBranchTree &tree, CerealReader &reader)
    : ctx(tree.ctx), tree(tree), is_frozen_(true) {
  reader >> *this;
}

// TODO: area_ or vars_ not initialized
SofaState::SofaState(SofaBranchTree &tree, const Json::Value &json) : 
    ctx(tree.ctx),
    tree(tree), 
    e_(ints_from_json(json["niche"])), 
    conds_(ints_from_json(json["constraints"])),
    id_(json["id"].asInt()),
    is_frozen_(true),
    is_valid_(json["valid"].asBool()) {
}

bool SofaState::is_valid() const { 
  return is_valid_;
}

int SofaState::id() const {
  return id_;
}

std::string SofaState::id_string() const {
  return "N" + std::to_string(id_);
}

QT SofaState::area() { 
  expect(!is_frozen_ && is_valid_);
  update_();
  return area_; 
}

std::vector<QT> SofaState::vars() { 
  expect(!is_frozen_ && is_valid_);
  update_();
  return vars_; 
}

void SofaState::impose(SofaConstraintProbe cond) {
  if (is_valid_) {
    conds_.push_back(cond);
    // Update only when current solution becomes invalid
    if (!ctx.ineq(cond)(vars_))
      update_();
  }
}

void SofaState::impose(const SofaConstraints &conds) {
  expect(!is_frozen_);

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
  expect(!is_frozen_);
  expect(is_valid_);

  int parent_id = this->id_;
  int child_left_id = tree.new_state_id_();
  int child_right_id = tree.new_state_id_();

  SofaState other(*this);
  this->impose(ineq);
  this->id_ = child_left_id;
  other.impose(-ineq);
  other.id_ = child_right_id;

  std::lock_guard<std::mutex> guard(tree.lock_);
  tree.split_states_.emplace_back(
    parent_id, ineq, child_left_id, child_right_id);

  return other;
}

const std::vector<int> &SofaState::e() const { 
  return e_; 
}

const std::vector<SofaConstraintProbe> &SofaState::conds() const { 
  return conds_; 
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
  expect(!is_frozen_);
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

Json::Value SofaState::json() const {
  Json::Value res(Json::objectValue);
  res["id"] = id_;
  res["niche"] = to_json(e());
  res["constraints"] = to_json(conds_);
  // TODO: add proof that this e is valid from constraints

  // TODO: 'valid' here means that the max area is > 2.2195
  // 'invalid' in sofa optimization result means that the constraints are contradictory 
  if (is_valid()) {
    res["valid"] = true;
  } else {
    res["valid"] = false;
    res["invalidity_proof"] = area_result_.json();
  }

  return res;
}

void SofaState::update_() {
  expect(!is_frozen_);
  area_result_ = sofa_area_qp(ctx.area(e_), ctx, conds_);
  if (area_result_) {
    is_valid_ = true;
    area_ = area_result_.optimality_proof().max_area;
    vars_ = area_result_.optimality_proof().maximizer;
    expect((ctx.area(e_))(vars_) == area_);
    expect(area_ > QT(22195, 10000));
  } else {
    is_valid_ = false;
    // state turned from valid to invalid
    std::lock_guard<std::mutex> guard(tree.lock_);
    tree.invalid_states_.push_back(*this);
  }
}