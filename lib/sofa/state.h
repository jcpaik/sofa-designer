#pragma once

#include <vector>

#include "number.h"
#include "forms.h"
#include "ineq.h"
#include "context.h"
#include "qp.h"

class CerealWriter;
class CerealReader;

class SofaState {
  public:
    // The context the state depends on
    const SofaContext &ctx;

    // Prevent SofaState with no context
    SofaState() = delete;
    // Constructors
    SofaState(const SofaContext &ctx, int i);
    SofaState(const SofaState &other);
    // Read from a file
    explicit SofaState(const SofaContext &ctx, const char *file);
    // Read from a stream
    explicit SofaState(const SofaContext &ctx, CerealReader &reader);

    // Prevent any assignment between states
    // No state is overwritten
    SofaState &operator=(const SofaState &other) = delete;
    SofaState &operator=(SofaState &&other) = delete;

    bool is_valid() const;
    QT area();
    std::vector<QT> vars();
    void impose(SofaConstraintProbe cond);
    void impose(const SofaConstraints &cond);

    // Impose condition `ineq` to current state
    // and return a new SofaState with the opposite of ineq imposed
    SofaState split(SofaConstraintProbe cond);

    // Accessors to polyline constructs
    const std::vector<int> &e() const;
    int e(int i) const;
    const LinearFormPoint &p(int i) const;
    Vector v(int i) const;

    // Update polyline
    void update_e(const std::vector<int> &e);

    // Check compatibility of current sofa with given inequality
    bool is_compatible(const LinearInequality &extra_ineq) const;
    bool is_compatible(const std::vector<LinearInequality> &extra_ineqs) const;

    // Read/write
    // Does not store/load context information
    friend CerealWriter &operator<<(CerealWriter &out, const SofaState &v);
    friend CerealReader &operator>>(CerealReader &in, SofaState &v);
    
  private:
    bool is_valid_;
    std::vector<int> e_; 
    SofaConstraints conds_;

    // A set of valid variables var_ and its corresponding area area_
    // It is NOT guaranteed that these attain the maximum area
    // These are not a part of serialized/unserialized members
    // When loaded, just call update_
    QT area_;
    std::vector<QT> vars_; 

    // Internally update area_ and var_ from given conditions 
    // Updated values are maximized area 
    void update_();
};

// --------inlines--------

inline SofaState::SofaState(const SofaContext &ctx, int i)
    : ctx(ctx), 
      is_valid_(true), 
      e_({0, i, i - ctx.n(), 0}), 
      conds_(ctx.default_constraints()) {
  conds_.push_back(ctx.is_over(i, i - ctx.n(), 0));
  update_();
}

inline SofaState::SofaState(const SofaState& s)
    : ctx(s.ctx), 
      is_valid_(s.is_valid_),
      e_(s.e_), 
      conds_(s.conds_), 
      area_(s.area_), 
      vars_(s.vars_) {
}

inline bool SofaState::is_valid() const { 
  return is_valid_;
}

inline QT SofaState::area() { 
  update_();
  return area_; 
}

inline std::vector<QT> SofaState::vars() { 
  update_();
  return vars_; 
}

inline SofaState SofaState::split(SofaConstraintProbe ineq) {
  SofaState other(*this);
  this->impose(ineq);
  // Inequality negation is operator-()
  other.impose(-ineq);
  return other;
}

inline const std::vector<int> &SofaState::e() const { 
  return e_; 
}

inline int SofaState::e(int i) const {
  return e_[i];
}

inline const LinearFormPoint &SofaState::p(int i) const { 
  return ctx.p(e(i), e(i+1));
}

inline Vector SofaState::v(int i) const {
  return p(i)(vars_);
}

inline void SofaState::update_e(const std::vector<int> &e) {
  if (is_valid_) {
    e_ = e;
    auto narea = (ctx.area(e_))(vars_);
    assert(area_ >= narea);
    area_ = narea;
    if (area_ < QT(22195, 10000)) // Optimization
      update_();
  }
}
