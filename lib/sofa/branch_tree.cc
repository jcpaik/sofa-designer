#include "branch_tree.h"

#include <cassert>
#include <iostream>
#include <thread>
#include <future>

#include "tqdm.h"

#include "branch_logic.h"
#include "cereal.h"


SofaBranchTree::SofaBranchTree(const SofaContext &ctx, int i)
    : ctx(ctx), n(ctx.n()) {
  valid_states_.emplace_back(ctx, i);
  // std::cout << valid_states_.back().is_valid() << std::endl;
  // std::cout << valid_states_.back().area() << std::endl;
}

SofaBranchTree::SofaBranchTree(const SofaContext &ctx, const char *file)
    : ctx(ctx) {
  load(file, *this);
}

const std::vector<SofaState> &SofaBranchTree::valid_states() const {
  return valid_states_;
}

std::vector<SofaState> process(
    std::vector<SofaState> states, int i, bool extend_in, bool extend_out, bool show_tqdm) {
  std::vector<SofaState> results;
  if (show_tqdm) {
    tqdm bar;
    int c = 0;
    int n = int(states.size());
    for (SofaState &s : states) {
      bar.progress(c, n);
      c++;
      ::add_corner(s, i, results, extend_in, extend_out);
    }
    bar.finish();
  } else {
    for (SofaState &s : states) {
      ::add_corner(s, i, results, extend_in, extend_out);
    }
  }
  return results;
}

void SofaBranchTree::add_corner(int i, bool extend_in, bool extend_out, int nthread) {
  assert(1 <= i && i < n);
  std::vector<SofaState> cur_states[nthread];
  for (int idx = 0; idx < int(valid_states_.size()); idx++)
    cur_states[idx % nthread].push_back(valid_states_[idx]);
  valid_states_.clear();
  // for each state, propagate
  std::future< std::vector<SofaState> > nxt_states[nthread];
  for (int rnk = 0; rnk < nthread; rnk++)
    nxt_states[rnk] = std::async(process, cur_states[rnk], i, extend_in, extend_out, rnk == 0);
  for (int rnk = 0; rnk < nthread; rnk++) {
    auto res = nxt_states[rnk].get();
    for (const auto &vv : res)
      valid_states_.push_back(vv);
  }
}
