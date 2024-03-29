#include <catch2/catch_all.hpp>

#include <iostream>

#include "sofa/context.h"
#include "sofa/branch_tree.h"

TEST_CASE( "Checking exactness of state", "[QP, STATE]" ) {
  SofaContext ctx( {
      {QT{2496,2545}, QT{497,2545}}, 
      {QT{12,13}, QT{5,13}}, {QT{3596,4325}, QT{2403,4325}},
      {QT{803761,1136689}, QT{803760,1136689}}, 
      {QT{2403,4325}, QT{3596,4325}}, 
      {QT{5,13}, QT{12,13}}, {QT{497,2545}, QT{2496,2545}}
      }
      );
  NT a("858413240226912435793494170653955337517692586819527086695188");
  NT b("347534368299812191344928277446711705522020432192595617308389");
  QT q(a, b);
  SofaBranchTree t(ctx);
  t.add_corner(4);
  SofaState s = t.valid_states()[0];
  s.update_e({0, 1, 2, 5, -3, 4, -4, 3, -5, -2, -1, 0});
  auto v = s.area();
  REQUIRE(v == q);

  NT c("7805398832952948167176940304917454904156956406964204460461049518363907\
356429");
  NT d("3164324140760781623487536824249536405049169003928100829275020961329526\
890507");

  s.impose(ctx.is_over(1, 2, 7));
  REQUIRE(s.area() == QT(c, d));
}
