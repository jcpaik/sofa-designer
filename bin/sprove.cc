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
    const QT &lb,
    Json::Value &sink) {
  auto res = v.is_compatible(val <= lb);
  if (!res) {
    auto &target = sink[v.id_string()];
    target["lower_bound"] = to_json(lb).asString();
    target["proof"] = res.json();
    return true;
  } else {
    return false;
  }
}

Json::Value find_lb(
    const SofaBranchTree &tree, 
    const LinearForm &val,
    QT lb, QT ub, int bsearch_depth,
    const std::string &out) {

  Json::Value output;
  QT res = ub;
  tqdm bar;
  int c = 0, n = tree.valid_states().size();
  for (const auto &v : tree.valid_states()) {
    bar.progress(c++, n); 
    bar.set_label(to_json(res).asString());
    
    if (report_incompatible(v, val, res, output))
      continue;

    QT clb = lb, cub = ub;
    for (int i = 0; i < bsearch_depth; i++) {
      QT md = (clb + cub) / 2;
      std::cout << to_json(md).asString() << std::endl;
      if (report_incompatible(v, val, md, output))
        clb = md;
      else
        cub = md;
    }

    assert(res > clb);
    if (!output.isMember(v.id_string()))
      throw std::runtime_error("Lower bound doesn't work");
    
    res = clb;
  }
  bar.finish();
  std::cout << to_json(res).asString();

  output["lower_bound"] = to_json(res).asString();

  return output;
}

void run(const std::string &treedir, const std::string &ineq, 
    QT lb, QT ub, const std::string &out) {
  std::filesystem::path treep(treedir);
  if (!std::filesystem::is_directory(treep)) {
    throw std::runtime_error("The tree directory does not exist");
  }
  
  Json::Value angles;
  {
    std::ifstream angles_f(treep / std::filesystem::path("angles.json"));
    angles_f >> angles;
    angles_f.close();
  }
  SofaContext ctx(angles);

  Json::Value leaf_nodes;
  {
    std::ifstream leaf_nodes_f(treep / std::filesystem::path("leaf-nodes.json"));
    leaf_nodes_f >> leaf_nodes;
    leaf_nodes_f.close();
  }
  SofaBranchTree tree(ctx, Json::Value(false), leaf_nodes);

  Parser parser(ctx);
  LinearForm val = parser.parse_expr(ineq);

  std::cout << ctx.n() << std::endl;
  for (const auto &v : tree.valid_states())
    std::cout << v.e().size() << std::endl;

  auto output = find_lb(tree, val, lb, ub, 5, out);
  output["value"] = ineq;

  std::ofstream ineq_f(out);
  ineq_f << output;
  ineq_f.close();
}

int main(int argc, char* argv[]) {
  try {
    std::string tree, ineq, lb_str, ub_str, out;

    // Set up syntax for arguments
    po::options_description desc("Allowed options");
    desc.add_options()
      ("help", "Produce help message")
      ("tree", po::value<std::string>(&tree), "Required tree output directory of sbranch")
      ("ineq", po::value<std::string>(&ineq), "Required inequality in string form")
      ("lb", po::value<std::string>(&lb_str), "Required inequality in string form")
      ("ub", po::value<std::string>(&ub_str), "Required inequality in string form")
      ("out", po::value<std::string>(&out)->implicit_value(""),
         "Directory for proof output (optional)\n")
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
