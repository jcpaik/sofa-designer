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

  return 0;
}
