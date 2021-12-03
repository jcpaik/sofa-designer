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

bool is_ub_true(const SofaState &s, const LinearForm &target, QT ub) {
  return !s.is_compatible(target >= LinearForm::constant(ctx.d(), ub));
}

QT find_ub(const LinearForm &target, QT lb, QT ub, int bsearch_depth) {
  QT res = lb;
  tqdm bar;
  int c = 0, n = t.valid_states().size();
  for (const auto &v : t.valid_states()) {
    bar.progress(c++, n); 
    bar.set_label(qt_to_str(res));
    
    if (is_ub_true(v, target, res))
      continue;

    QT clb = lb, cub = ub;
    for (int i = 0; i < bsearch_depth; i++) {
      QT md = (clb + cub) / 2;
      if (is_ub_true(v, target, md))
        cub = md;
      else
        clb = md;
    }

    assert(res < cub);
    res = cub;
  }
  bar.finish();
  return res;
}

int main() {
  for (int i = 0; i <= ctx.n() - 1; i++) {
	  auto res = find_ub(dot(ctx.u(i), ctx.A(i) - ctx.C(i)) + ctx.s(ctx.n() + i + 1), 0, 4, 10);
    std::cout << "Result: " << res << std::endl;
  }

  return 0;
}
