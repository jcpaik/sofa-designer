#pragma once

#include <mutex>
#include <vector>

#include "number.h"
#include "forms.h"
#include "ineq.h"
#include "context.h"
#include "qp.h"
#include "state.h"

class SofaBranchTree {
  public:
    const SofaContext &ctx;

    // Tree with initial search
    SofaBranchTree(const SofaContext &ctx, int i);
    // Load from file
    explicit SofaBranchTree(const SofaContext &ctx, const char *file);
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

    friend CerealWriter &operator<<(CerealWriter &out, 
                                    const SofaBranchTree &v);
    friend CerealReader &operator>>(CerealReader &in, SofaBranchTree &v);

    int new_state_id();
 
  private:
    int n;
    // List of indices unioned so far with `add_corner`
    std::vector<int> indices_;
    std::vector<SofaState> valid_states_;

    std::mutex lock_;
    int last_state_id_;
};
