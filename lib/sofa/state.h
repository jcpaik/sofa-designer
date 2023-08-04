#pragma once

#include <vector>

#include <json/json.h>

#include "number.h"
#include "forms.h"
#include "ineq.h"
#include "context.h"
#include "qp.h"

class SofaBranchTree;

// Invariants: 
// - Constraints are only added, never subtracted
// - Updated area function (update_e) is monotonically smaller than
//   the previous function
class SofaState {
  public:
    // The context the state depends on
    const SofaContext &ctx;
    SofaBranchTree &tree;

    // Prevent SofaState with no context
    SofaState() = delete;
    // TODO: remove copy constructor
    SofaState(const SofaState &other);

    // Prevent any assignment between states
    // No state is overwritten
    SofaState &operator=(const SofaState &other) = delete;
    SofaState &operator=(SofaState &&other) = delete;

    bool is_valid() const;

    // Accessors to polyline constructs
    const std::vector<int> &e() const;
    int e(int i) const;
    const LinearFormPoint &p(int i) const;
    Vector v(int i) const;

    // Only these methods change the state of the node
    void impose(SofaConstraintProbe cond);
    void impose(const SofaConstraints &cond);
    // Impose condition `ineq` to current state
    // and return a new SofaState with the opposite of ineq imposed
    SofaState split(SofaConstraintProbe cond);
    // Update polyline
    void update_e(const std::vector<int> &e);

    // Guaranteed maximum area and maximizer
    QT area();
    std::vector<QT> vars();

    SofaAreaResult is_compatible(
      const LinearInequality &extra_ineq) const;
    SofaAreaResult is_compatible(
      const std::vector<LinearInequality> &extra_ineqs) const;

    Json::Value json() const;
    
  private:
    friend class SofaBranchTree;

    // Constructors
    SofaState(SofaBranchTree &tree);

    int id_;

    std::vector<int> e_; 
    SofaConstraints conds_;

    // If state is invalid, contains a correct proof of invalidity
    // If valid, `area_result_` may not contain a correct proof of optimality
    // but `area_` and `vars_` always contain a valid assignment
    SofaAreaResult area_result_;
    QT area_;
    std::vector<QT> vars_; 

    // Called if and only if the state changes its value
    void update_();
};
