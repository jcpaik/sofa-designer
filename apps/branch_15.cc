#include "sofa/context.h"
#include "sofa/branch_tree.h"
#include "sofa/cereal.h"

void debug_states(std::vector<SofaState> valid_states) {
  
  std::cout << valid_states.size() << " states" << std::endl;
  /*
  QT maxarea = 0;
  auto maxs = &valid_states[0];
  for (auto &s : valid_states) {
    auto aa = s.area();
    if (maxarea < aa) {
      maxarea = aa;
      maxs = &s;
    }
  }
  std::cout << "area " << maxs->area() << std::endl;
  for (auto ee : maxs->e())
    std::cout << ee << " ";
  std::cout << std::endl;
  std::cout << "{";
  for (auto val : maxs->vars())
    std::cout << val << ", ";
  std::cout << "}" << std::endl;
  std::cout << "{" << std::endl;
  for (int i = 0; i <= int(maxs->e().size()) - 2; i++) {
    std::cout << '{' << maxs->v(i).x << ", " << maxs->v(i).y << "}, " << std::endl;
  }
  std::cout << "}" << std::endl;
  for (auto &s : valid_states) {
    for (auto ee : s.e())
      std::cout << ee << " ";
    std::cout << std::endl;
  }
  */
}

int main() {
  SofaContext ctx({
      {QT{364, 365},QT{27, 365}},
      {QT{144, 145},QT{17, 145}}, 
      {QT{3420, 3469},QT{581, 3469}},
      {QT{1911, 1961},QT{440, 1961}}, // 4
      {QT{4941, 5141},QT{1420, 5141}}, 
      {QT{9711, 10289},QT{3400, 10289}}, 
      {QT{12, 13},QT{5, 13}}, 
      {QT{85608, 95017},QT{41225, 95017}}, // 8
      {QT{1596, 1885},QT{1003, 1885}},
      {QT{351, 449},QT{280, 449}}, // 10
      {QT{803760, 1136689},QT{803761, 1136689}},
      {QT{280, 449},QT{351, 449}}, // 12
      {QT{41225, 95017},QT{85608, 95017}}, // 13
      {QT{440, 1961},QT{1911, 1961}}, // 14
      });

  save("ctx15.crl", ctx);

  const int num_cpu = 32;

  SofaBranchTree t(ctx, 10);
  debug_states(t.valid_states());
  t.add_corner(12, false, true, num_cpu);
  debug_states(t.valid_states());
  t.add_corner(8, false, true, num_cpu);
  debug_states(t.valid_states());
  t.add_corner(13, false, true, num_cpu);
  debug_states(t.valid_states());
  t.add_corner(4, false, true, num_cpu);
  debug_states(t.valid_states());
  t.add_corner(14, false, true, num_cpu);
  debug_states(t.valid_states());

  save("bt15.crl", t);

  return 0;
}
