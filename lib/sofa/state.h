#pragma once

#include <vector>

#include <json/json.h>

#include "number.h"
#include "forms.h"
#include "ineq.h"
#include "context.h"
#include "qp.h"

class CerealWriter;
class CerealReader;
class SofaBranchTree;

class SofaState {
  public:
    // The context the state depends on
    const SofaContext &ctx;
    const SofaBranchTree &tree;

    // Prevent SofaState with no context
    SofaState() = delete;
    // TODO: remove copy constructor
    SofaState(const SofaState &other);

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
    friend CerealReader &operator>>(CerealReader &in, SofaBranchTree &v);

    Json::Value json();
    
  private:
    friend class SofaBranchTree;

    // Constructors
    SofaState(const SofaBranchTree &tree, int i);
    // Read from a file
    explicit SofaState(const SofaBranchTree &tree, const char *file);
    // Read from a stream
    explicit SofaState(const SofaBranchTree &tree, CerealReader &reader);

    int id_;

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
