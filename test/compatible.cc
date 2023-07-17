#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_all.hpp>

#include <iostream>

#include "sofa/context.h"
#include "sofa/branch_tree.h"
#include "sofa/qp.h"

TEST_CASE( "Checking compatibility", "[CMP]" ) {
  SofaContext ctx( {
      {QT{2496,2545}, QT{497,2545}}, 
      {QT{12,13}, QT{5,13}}, {QT{3596,4325}, QT{2403,4325}},
      {QT{803761,1136689}, QT{803760,1136689}}, 
      {QT{2403,4325}, QT{3596,4325}}, 
      {QT{5,13}, QT{12,13}}, {QT{497,2545}, QT{2496,2545}}
      }
      );
  SofaBranchTree t(ctx, 4);
  SofaState s = t.valid_states()[0];
  s.update_e({0, 1, 2, 5, -3, 4, -4, 3, -5, -2, -1, 0}); 
  NT a("858413240226912435793494170653955337517692586819527086695188");
  NT b("347534368299812191344928277446711705522020432192595617308389");
  QT q(a, b);
  REQUIRE(s.area() == q);

  REQUIRE(s.is_compatible(ctx.s(ctx.n()) >= LinearForm::constant(ctx.d(), 1)));
  REQUIRE(!s.is_compatible(ctx.s(ctx.n()) >= LinearForm::constant(ctx.d(), 10)));

  /*
  BENCHMARK("area computation") {
    ctx.area({0, 1, 2, 5, -3, 4, -4, 3, -5, -2, -1, 0}); 
  };
  BENCHMARK("area with qp") {
    auto v = MaximizeQuadraticFormNonnegativeVariables(
        ctx.area({0, 1, 2, 5, -3, 4, -4, 3, -5, -2, -1, 0}), 
        ctx.default_constraints()).value;
  };
  */
}
