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

SofaContext ctx("ctx15.crl");
SofaBranchTree t(ctx, "bt15.crl");
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
  auto zero = LinearForm::constant(ctx.d(), 0);
  auto one = LinearForm::constant(ctx.d(), 1);
  auto w = LinearFormPoint(-one, zero);
  auto w_vec = Vector(-1, 0);

  // 8
  for (int i = 7; i >= 1; i--) {
    auto glb = dot(ctx.v(i), ctx.C(i) - ctx.A(i+2));
    auto gw = dot(-ctx.u(i+1), w_vec);
    auto hlb = dot(ctx.u(i+1), ctx.A(i+1) - ctx.C(i-1));
    auto hw = dot(ctx.v(i), w_vec);
    auto c = dot(ctx.u(i+1) - ctx.v(i+1), w);
    auto res = find_lb(gw * glb + hw * hlb + c, -8, 8, 10);
    std::cout << "Result: " << res << std::endl;
  }

  return 0;
}
