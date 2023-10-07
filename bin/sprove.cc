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
#include <shared_mutex>
#include <thread>
#include <future>

#include "sofa/context.h"
#include "sofa/geom.h"
#include "sofa/branch_tree.h"
#include "sofa/json.h"
#include "sofa/cereal.h"
#include "parse.h"
#include "tqdm.h"

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

std::shared_mutex mutex;
QT lb, lb_min, lb_max;
int bsearch_depth = 5;

void find_lb_worker(
    int worker_id, int num_workers,
    const std::vector<SofaState> &nodes, 
    const LinearForm &val) {
  tqdm *bar = worker_id ? nullptr : new tqdm();
  int c = 0, n = (int(nodes.size()) - 1) / num_workers + 1;
  for (int i = worker_id; i < nodes.size(); i += num_workers) {
    const auto &v = nodes[i];
    if (bar)
      bar->progress(c++, n);

    mutex.lock_shared();
    if (bar)
      bar->set_label(to_json(lb).asString());
    
    if (!report_incompatible(v, val, lb)) {
      mutex.unlock_shared();
      mutex.lock();
      if (!report_incompatible(v, val, lb)) {
        bool lb_found = false;
        QT clb = lb_min, cub = lb_max;
        for (int i = 0; i < bsearch_depth; i++) {
          QT md = (clb + cub) / 2;
          if (report_incompatible(v, val, md))
            lb_found = true, clb = md;
          else
            cub = md;
        }
        if (!lb_found)
          throw std::runtime_error("Lower bound invalid");
        expect(lb > clb);
        lb = clb;
      }
      mutex.unlock();
    }
    else
      mutex.unlock_shared();
  }
  if (bar)
    bar->finish();
}

void find_lb(
    const SofaBranchTree &tree, 
    const LinearForm &val,
    QT min, QT max,
    const std::string &out,
    int nthread) {
  lb_min = min;
  lb = lb_max = max;

  const auto &nodes = tree.valid_states();

  std::future<void> futures[nthread];
  for (int i = 0; i < nthread; i++)
    futures[i] = std::async(find_lb_worker, i, nthread, nodes, val);
  for (auto &f : futures)
    f.get();
  
  std::cout << to_json(lb).asString() << std::endl;
}

void run(const std::string &treefile, const std::string &ineq, 
    QT lb, QT ub, const std::string &out, int nthreads) {

  CerealReader reader(treefile.c_str());
  SofaContext ctx(reader);
  SofaBranchTree tree(ctx, reader);
  // expect(reader.eof());
  reader.close();

  Parser parser(ctx);
  LinearForm val = parser.parse_expr(ineq);

  find_lb(tree, val, lb, ub, out, nthreads);

  // TODO: json output functionality
}

int main(int argc, char* argv[]) {
  try {
    std::string tree, ineq, lb_str, ub_str, out;
    int nthreads;

    // Set up syntax for arguments
    po::options_description desc("Allowed options");
    desc.add_options()
      ("help", "Produce help message")
      ("tree", po::value<std::string>(&tree), "Required tree cereal output of sbranch")
      ("ineq", po::value<std::string>(&ineq), "Required inequality in string form")
      ("lb", po::value<std::string>(&lb_str), "Required inequality in string form")
      ("ub", po::value<std::string>(&ub_str), "Required inequality in string form")
      ("out", po::value<std::string>(&out)->default_value(""),
         "Directory for proof output in json (optional)\n")
      ("nthreads", po::value<int>(&nthreads)->default_value(1),
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

    run(tree, ineq, stoq(lb_str), stoq(ub_str), out, nthreads);
  } catch(std::exception& e) {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  } catch(...) {
    std::cerr << "Exception of unknown type!\n";
    return 1;
  }

  return 0;
}
