#pragma once

#include <mutex>
#include <vector>

#include "number.h"
#include "forms.h"
#include "ineq.h"
#include "context.h"
#include "qp.h"
#include "state.h"

struct SplitState {
  int id;
  SofaConstraintProbe split_by;
  int child_left_id;
  int child_right_id;

  SplitState(
    int id, 
    SofaConstraintProbe split_by, 
    int child_left_id, 
    int child_right_id) :
      id(id),
      split_by(split_by),
      child_left_id(child_left_id),
      child_right_id(child_right_id) {}
};

class SofaBranchTree {
  public:
    const SofaContext &ctx;

    // Tree with initial search
    SofaBranchTree(const SofaContext &ctx, int i);
    // TODO: Tree from a pre-computed root
    SofaBranchTree(const SofaState &root);

    // Not default-initializable. Not assignable.
    SofaBranchTree() = delete;
    SofaBranchTree &operator=(const SofaBranchTree &other) = delete;
    SofaBranchTree &operator=(SofaBranchTree &&other) = delete;

    // Current leaves of the branch-and-bound tree
    const std::vector<SofaState> &valid_states() const;

    // Runs a branch-and-bound algorithm by adding i'th corner
    void add_corner(int i, bool extend = true, int nthread = 1);  

    // TODO: Sets tqdm visibility
    void show_tqdm(bool flag);

  private:
    int n;
    // List of indices unioned so far with `add_corner`
    std::vector<int> indices_;
    std::vector<SofaState> valid_states_;

    // Dead states
    std::vector<SofaState> invalid_states_;

    // Splitting information
    std::mutex lock_;
    int last_state_id_;
    int new_state_id_();

    std::vector<SplitState> split_states_;

    friend SofaState SofaState::split(SofaConstraintProbe cond);
    friend void SofaState::update_();
};
