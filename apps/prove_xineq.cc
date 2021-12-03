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

SofaContext ctx("ctx14.crl");
SofaBranchTree t(ctx, "bt14.crl");
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

void check(int i, int j) {
  auto vj = ctx.x(j).y - ctx.x(j).x;
  auto vi = ctx.x(i).y - ctx.x(i).x;
  auto res = find_lb(vj - vi, 0, 4, 10);
  std::cout << "Result: " << i << " " << j << " " << res << std::endl;
}

int main() {
  check(0, 4);
  check(1, 4);
  check(2, 5);

  return 0;
}
