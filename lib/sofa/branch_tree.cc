#include "branch_tree.h"

#include <iostream>
#include <thread>
#include <future>

#include "tqdm.h"

#include "expect.h"
#include "branch_logic.h"
#include "cereal.h"

SofaBranchTree::SofaBranchTree(const SofaContext &ctx)
    : ctx(ctx), last_state_id_(0) {
  valid_states_.push_back(SofaState(*this));
  // std::cout << valid_states_.back().is_valid() << std::endl;
  // std::cout << valid_states_.back().area() << std::endl;
}

SofaBranchTree::SofaBranchTree(const SofaContext &ctx, CerealReader &reader)
    : ctx(ctx), last_state_id_(0) {
  reader >> *this;
}

SofaBranchTree::SofaBranchTree(
    const SofaContext &ctx,
    const Json::Value &split_nodes,
    const Json::Value &leaf_nodes)
    : ctx(ctx), last_state_id_(0) {
  // don't update split_nodes
  // don't keep track of last ID

  for (const auto &leaf : leaf_nodes) {
    // added to tree as generated
    SofaState state(*this, leaf);
  }
}

const std::vector<SofaState> &SofaBranchTree::valid_states() const {
  return valid_states_;
}

std::vector<SofaState> process(
    std::vector<SofaState> states, int i, bool extend, bool show_tqdm) {
  std::vector<SofaState> results;
  if (show_tqdm) {
    tqdm bar;
    int c = 0;
    int n = int(states.size());
    for (SofaState &s : states) {
      bar.progress(c, n);
      c++;
      ::add_corner(s, i, results, extend);
    }
    bar.finish();
  } else {
    for (SofaState &s : states) {
      ::add_corner(s, i, results, extend);
    }
  }
  return results;
}

void SofaBranchTree::add_corner(int i, bool extend, int nthread) {
  expect(1 <= nthread);

  int n = ctx.n();
  expect(1 <= i && i < n);

  // split valid states into equal-sized chunks
  int nstate = int(valid_states_.size());
  std::vector< std::vector<SofaState> > cur_states;
  int start_idx = 0;
  for (int rnk = 0; rnk < nthread; rnk++) {
    int end_idx = start_idx + (nstate / nthread) + int(rnk < nstate % nthread);
    cur_states.emplace_back(
        valid_states_.begin() + start_idx,
        valid_states_.begin() + end_idx);
    start_idx = end_idx;
  }
  valid_states_.clear();

  // run the algorithm on each chunk asynchronously
  std::future< std::vector<SofaState> > nxt_states[nthread];
  for (int rnk = 0; rnk < nthread; rnk++)
    nxt_states[rnk] = std::async(process, cur_states[rnk], i, extend, rnk == 0);
  for (const auto &res : nxt_states) {
    auto vec = res.get();
    valid_states_.insert(valid_states_.end(), vec.begin(), vec.end());
  }
  expect(split_states_.size() + 1 == valid_states_.size() + invalid_states_.size());
}

Json::Value SofaBranchTree::split_nodes() const {
  Json::Value res;

  for (auto const &split : split_states_) {
    auto &val = res["N" + std::to_string(split.id)];
    val["split_by"] = split.split_by;
    val["left"] = "N" + std::to_string(split.child_left_id);
    val["right"] = "N" + std::to_string(split.child_right_id);
  }

  return res;
}

Json::Value SofaBranchTree::leaf_nodes() const {
  Json::Value res;

  for (auto const &leaf : valid_states_) {
    auto &val = res["N" + std::to_string(leaf.id_)];
    val = leaf.json();
  }

  for (auto const &leaf : invalid_states_) {
    auto &val = res["N" + std::to_string(leaf.id_)];
    val = leaf.json();
  }

  return res;
}

int SofaBranchTree::new_state_id_() {
  std::lock_guard<std::mutex> guard(lock_);
  last_state_id_++;
  return last_state_id_;
}
