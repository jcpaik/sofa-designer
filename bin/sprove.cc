#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <iterator>
#include <istream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <stdexcept>

#include "sofa/context.h"
#include "sofa/geom.h"
#include "sofa/branch_tree.h"
#include "sofa/json.h"
#include "sofa/cereal.h"
#include "parse.h"
#include "tqdm.h"

/*
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
*/

bool report_incompatible(
    const SofaState &v,
    const LinearForm &val,
    const QT &lb) {
  auto res = v.is_compatible(val <= lb);
  if (!res) {
    return true;
  } else {
    return false;
  }
}

void find_lb(
    const SofaBranchTree &tree, 
    const LinearForm &val,
    QT lb, QT ub, int bsearch_depth,
    const std::string &out) {

  QT res = ub;
  tqdm bar;
  int c = 0, n = tree.valid_states().size();
  for (const auto &v : tree.valid_states()) {
    bar.progress(c++, n);
    bar.set_label(to_json(res).asString());
    
    if (!report_incompatible(v, val, res)) {
      QT clb = lb, cub = ub;
      for (int i = 0; i < bsearch_depth; i++) {
        QT md = (clb + cub) / 2;
        if (report_incompatible(v, val, md))
          clb = md;
        else
          cub = md;
      }
      res = clb;
      expect(report_incompatible(v, val, res));
    }
  }
  bar.finish();
  std::cout << to_json(res).asString();
}

void run(const std::string &treefile, const std::string &ineq, 
    QT lb, QT ub, const std::string &out) {

  CerealReader reader(treefile.c_str());
  SofaContext ctx(reader);
  SofaBranchTree tree(ctx, reader);
  // expect(reader.eof());
  reader.close();

  Parser parser(ctx);
  LinearForm val = parser.parse_expr(ineq);

  find_lb(tree, val, lb, ub, 5, out);

  // TODO: json output functionality
}

int main(int argc, char* argv[]) {
  try {
    std::string tree, ineq, lb_str, ub_str, out;

    // Set up syntax for arguments
    po::options_description desc("Allowed options");
    desc.add_options()
      ("help", "Produce help message")
      ("tree", po::value<std::string>(&tree), "Required tree cereal output of sbranch")
      ("ineq", po::value<std::string>(&ineq), "Required inequality in string form")
      ("lb", po::value<std::string>(&lb_str), "Required inequality in string form")
      ("ub", po::value<std::string>(&ub_str), "Required inequality in string form")
      ("out", po::value<std::string>(&out)->implicit_value(""),
         "Directory for proof output in json (optional)\n")
      ;

    po::positional_options_description p;
    p.add("tree", 1);
    p.add("ineq", 1);
    p.add("lb", 1);
    p.add("ub", 1);

    // Parse arguments
    po::variables_map vm;        
    po::store(po::command_line_parser(argc, argv).
              options(desc).positional(p).run(), vm);
    po::notify(vm);    

    // Logic
    if (vm.count("help")) {
      std::cout << desc << "\n";
      return 0;
    }

    run(tree, ineq, stoq(lb_str), stoq(ub_str), out);
  } catch(std::exception& e) {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  } catch(...) {
    std::cerr << "Exception of unknown type!\n";
    return 1;
  }

  return 0;
}
