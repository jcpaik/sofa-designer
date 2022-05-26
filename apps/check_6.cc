#include <cassert>
#include <iostream>
#include <string>

#include "sofa/context.h"
#include "sofa/branch_tree.h"
#include "sofa/cereal.h"

SofaContext ctx("a6.ctx");
SofaBranchTree t(ctx, "a6.sofas");

int main() {
  std::vector<SofaState> valid_states = t.valid_states();
  QT maxarea = 0;
  auto maxs = &valid_states[0];
  int i = 0;
  for (auto &s : valid_states) {
    auto aa = s.area();
    i++;
    if (i % 100 == 0)
      std::cout << i << "/" << valid_states.size() << std::endl;
    if (maxarea < aa) {
      maxarea = aa;
      maxs = &s;
    }
  }
  std::cout << "area " << maxs->area() << std::endl;
  return 0;
}
