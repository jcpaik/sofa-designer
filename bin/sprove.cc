#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <iterator>
#include <istream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>

#include "sofa/context.h"
#include "sofa/geom.h"
#include "sofa/branch_tree.h"
#include "sofa/cereal.h"
#include "parse.h"

int main(int argc, char* argv[]) {
  SofaContext ctx({
      {QT{12,13},QT{5,13}},
      {QT{4,5},QT{3,5}},
      {QT{3,5},QT{4,5}},
      {QT{5,13},QT{12,13}}
      });

  Parser p(ctx);
  auto res = p.parse_expr("dot(A(0)-A(5),u(0))");
  assert (res == dot(ctx.u(0), ctx.A(0) - ctx.A(ctx.n())));
  for (int i = 1; i < ctx.n(); i++) {
    auto res = p.parse_expr(std::string("dot(A(0)-p(0,-")+std::to_string(i)+"),u(0))");
    assert (res == dot(ctx.u(1), ctx.A(0) - ctx.p(0, i)));
  }

  return 0;
}
