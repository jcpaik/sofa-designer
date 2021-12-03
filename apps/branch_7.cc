#include "sofa/context.h"
#include "sofa/branch_tree.h"
#include "sofa/cereal.h"

void debug_states(std::vector<SofaState> valid_states) {
  std::cout << valid_states.size() << " states" << std::endl;
}

int main() {
  SofaContext ctx({
      {QT{1911,1961},QT{440,1961}},
      {QT{85608,95017},QT{41225,95017}},
      {QT{351,449},QT{280,449}},
      {QT{803761,1136689},QT{803760,1136689}},
      {QT{280,449},QT{351,449}},
      {QT{41225,95017},QT{85608,95017}},
      {QT{440,1961},QT{1911,1961}}
      });

  save("ctx7.crl", ctx);

  SofaBranchTree t(ctx, 3);
  debug_states(t.valid_states());
  t.add_corner(5);
  debug_states(t.valid_states());
  t.add_corner(2);
  debug_states(t.valid_states());
  t.add_corner(6);
  debug_states(t.valid_states());
  t.add_corner(1);
  debug_states(t.valid_states());
  t.add_corner(7);
  debug_states(t.valid_states());

  save("bt7.crl", t);

  return 0;
}
