#include <catch2/catch_all.hpp>

#include <algorithm>
#include <iostream>

#include "sofa/number.h"
#include "sofa/context.h"
#include "sofa/qp.h"
#include "sofa/branch_tree.h"
#include "sofa/json.h"

LinearForm linearForm(Json::Value val) {
  return LinearForm(qt_from_json(val["b"]), qts_from_json(val["a"]));
}

TEST_CASE( "JSON certificate export of splitting values", "[CERTIFICATE]" ) {
  SofaContext ctx( {
      {QT{2496,2545}, QT{497,2545}}, 
      {QT{12,13}, QT{5,13}}, {QT{3596,4325}, QT{2403,4325}},
      {QT{803761,1136689}, QT{803760,1136689}}, 
      {QT{2403,4325}, QT{3596,4325}}, 
      {QT{5,13}, QT{12,13}}, {QT{497,2545}, QT{2496,2545}}
      }
      );

  int n = ctx.n();
  int idx = 1;

  Json::Value vals = ctx.split_values();
  REQUIRE(vals[0].isNull());

  for (int i = 0; i <= 2 * n; i++)
    REQUIRE(linearForm(vals[idx++]) == dot(ctx.A(i + 1) - ctx.A(i), ctx.v(i)));
  
  for (int l = 1; l <= (n - 1); l++)
    for (int j = -(n - 1); j <= (n - 1); j++)
      for (int i = -(n - 1); i < j; i++) {
        REQUIRE(linearForm(vals[idx++]) == ctx.p(l - n, l).x - ctx.p(i, j).x);
      }
  
  for (int k = -(n - 1); k <= (n - 1); k++)
    for (int j = -(n - 1); j < k; j++)
      for (int i = -(n - 1); i < j; i++) {
        REQUIRE(linearForm(vals[idx++]) == dot(ctx.p(i, j), ctx.v(k)) -
          ctx.s(k + n) + LinearForm::constant(ctx.d(), 1));
      }
}