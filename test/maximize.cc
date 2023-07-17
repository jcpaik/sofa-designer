#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_all.hpp>

#include <iostream>

#include "sofa/context.h"
#include "sofa/qp.h"

TEST_CASE( "Checking exactness of QP", "[QP]" ) {
  SofaContext ctx( {
      {QT{2496,2545}, QT{497,2545}}, 
      {QT{12,13}, QT{5,13}}, {QT{3596,4325}, QT{2403,4325}},
      {QT{803761,1136689}, QT{803760,1136689}}, 
      {QT{2403,4325}, QT{3596,4325}}, 
      {QT{5,13}, QT{12,13}}, {QT{497,2545}, QT{2496,2545}}
      }
      );
  auto v = sofa_area_qp(
      ctx.area({0, 1, 2, 5, -3, 4, -4, 3, -5, -2, -1, 0}), 
      ctx,
      ctx.default_constraints()).optimality_proof().max_area;
  NT a("858413240226912435793494170653955337517692586819527086695188");
  NT b("347534368299812191344928277446711705522020432192595617308389");
  QT q(a, b);
  REQUIRE(v == q);

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
