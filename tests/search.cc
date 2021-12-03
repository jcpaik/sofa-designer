#include "catch.hpp"

#include <algorithm>
#include <iostream>

#include "sofa/number.h"
#include "sofa/context.h"
#include "sofa/qp.h"
#include "sofa/branch_tree.h"

TEST_CASE( "Search tree", "[.][SEARCH]" ) {
  SofaContext ctx( {
      {QT{2496,2545}, QT{497,2545}}, 
      {QT{12,13}, QT{5,13}}, {QT{3596,4325}, QT{2403,4325}},
      {QT{803761,1136689}, QT{803760,1136689}}, 
      {QT{2403,4325}, QT{3596,4325}}, 
      {QT{5,13}, QT{12,13}}, {QT{497,2545}, QT{2496,2545}}
      }
      );

  SofaBranchTree t(ctx, 4);
  std::cout << t.valid_states().size() << std::endl;
  t.add_corner(2);
  std::cout << t.valid_states().size() << std::endl;
  t.add_corner(6);
  std::cout << t.valid_states().size() << std::endl;
  t.add_corner(3);
  std::cout << t.valid_states().size() << std::endl;
  t.add_corner(5);
  std::cout << t.valid_states().size() << std::endl;
  QT marea(0);
  auto x(t.valid_states());
  for (auto &s : x) {
    auto a = s.area();
    marea = std::max(marea, a);
  }
  std::cout << t.valid_states().size() << std::endl;
  std::cout << "area " << marea << std::endl;
  /*
  t.add_corner(7);
  std::cout << t.valid_states().size() << std::endl;
  */
}
