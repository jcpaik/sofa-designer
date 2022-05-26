#include <cassert>
#include <iostream>
#include <string>

#include "tqdm.h"

#include "sofa/context.h"
#include "sofa/branch_tree.h"
#include "sofa/cereal.h"

std::string qt_to_str(QT v) {
  return v.numerator().get_str() + "/" + v.denominator().get_str();
}

SofaContext ctx("a6.ctx");
SofaBranchTree t(ctx, "a6.sofas");
using StateIter = std::vector<SofaState>::const_iterator;

bool is_lb_true(const SofaState &s, const LinearForm &target, QT lb) {
  return !s.is_compatible(target <= LinearForm::constant(ctx.d(), lb));
}

QT find_lb(const LinearForm &target, QT lb, QT ub, int bsearch_depth) {
  QT res = ub;
  tqdm bar;
  int c = 0, n = t.valid_states().size();
  for (const auto &v : t.valid_states()) {
    bar.progress(c++, n); 
    bar.set_label(qt_to_str(res));
    
    if (is_lb_true(v, target, res))
      continue;

    QT clb = lb, cub = ub;
    for (int i = 0; i < bsearch_depth; i++) {
      QT md = (clb + cub) / 2;
      if (is_lb_true(v, target, md))
        clb = md;
      else
        cub = md;
    }

    assert(res > clb);
    res = clb;
  }
  bar.finish();
  return res;
}

int main() {
  for (int i = 1; i < ctx.n(); i++) {
    auto res = find_lb(dot(ctx.u(0), ctx.A(0) - ctx.p(0, -i)), 0, 4, 10);
    std::cout << "Result: " << res << std::endl;
  }
  auto res = find_lb(dot(ctx.u(0), ctx.A(0) - ctx.A(ctx.n())), 0, 4, 10);
  std::cout << "Result: " << res << std::endl; // 75/256

  return 0;
}
