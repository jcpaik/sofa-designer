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

int main() {
  auto gg = dot(ctx.u(1), ctx.A(1) - ctx.C(0)) - LinearForm::constant(ctx.d(), 1);
  auto hh = dot(ctx.v(0), ctx.C(0) - ctx.A(2)) - LinearForm::constant(ctx.d(), 1);
  auto res = find_lb(gg + hh, 0, 8, 10);
  std::cout << "Edge Result: " << res << std::endl;
  for (int i = 0; i <= 8; i++) {
    auto gg = dot(ctx.u(i + 2), ctx.A(i + 2) - ctx.C(i)) - LinearForm::constant(ctx.d(), 1);
    auto hh = dot(ctx.v(i + 1), ctx.C(i + 1) - ctx.A(i + 3)) - LinearForm::constant(ctx.d(), 1);
    auto res = find_lb(gg + hh, 0, 8, 10);
    std::cout << "Result: " << res << std::endl;
  }

  return 0;
}
