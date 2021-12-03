#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"

#include <iostream>

#include "sofa/context.h"
#include "sofa/qp.h"

TEST_CASE( "Checking exactness of is_negative_semidefinite", "[QP]" ) {
  REQUIRE(is_negative_semidefinite({
        {-1},
        {1, -1},
        {-1, 1, -2}
        }));

  REQUIRE(!is_negative_semidefinite({
        {1},
        {1, -1},
        {-1, -1, -2}
        }));

  SofaContext ctx( {
      {QT{2496,2545}, QT{497,2545}}, 
      {QT{12,13}, QT{5,13}}, {QT{3596,4325}, QT{2403,4325}},
      {QT{803761,1136689}, QT{803760,1136689}}, 
      {QT{2403,4325}, QT{3596,4325}}, 
      {QT{5,13}, QT{12,13}}, {QT{497,2545}, QT{2496,2545}}
      }
      );

  REQUIRE(is_negative_semidefinite(ctx.area({
          0, 1, 2, 5, -3, 4, -4, 3, -5, -2, -1, 0
          })));
  REQUIRE(is_negative_semidefinite(ctx.area({
          0, 4, -4, 0
          })));

  /*
  auto a = ctx.area({
          0, 1, 2, 5, -3, 4, -4, 3, -5, -2, -1, 0
          });

  BENCHMARK("is negative semidefinite") {
    REQUIRE(is_negative_semidefinite(a));
  };
  */

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
