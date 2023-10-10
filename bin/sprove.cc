#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <cstdlib>
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

struct Config {
  std::string tree_file_path;
  std::string linear_form;
  QT bound_min, bound_max;
  int bsearch_depth;
  int nthreads;
  bool find_lb, find_ub;
  std::string json_export_path;
};

Config parse_config(int argc, char* argv[]) {
  std::string tree, value, min_str, max_str, json_out;
  int nthreads, bsearch_depth;

  // Set up syntax for arguments
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "Produce help message")
    ("tree", po::value<std::string>(&tree), "Required tree cereal output of sbranch")
    ("value", po::value<std::string>(&value), "Required value in string form")
    ("min", po::value<std::string>(&min_str), "Required minimum value of lower/upper bound for binary search")
    ("max", po::value<std::string>(&max_str), "Required minimum value of lower/upper bound for binary search")
    ("lb", "Find lower bound")
    ("ub", "Find upper bound")
    ("json", po::value<std::string>(&json_out)->default_value(""),
      "Directory for proof output in json (optional)\n")
    ("nthreads", po::value<int>(&nthreads)->default_value(1),
      "Number of threads to use (optional)\n")
    ("bsearch-depth", po::value<int>(&bsearch_depth)->default_value(5),
      "Depth of binary search (optional)\n")
    ;

  po::positional_options_description p;
  p.add("tree", 1);
  p.add("ineq", 1);
  p.add("min", 1);
  p.add("max", 1);

  // Parse arguments
  po::variables_map vm;        
  po::store(po::command_line_parser(argc, argv).
            options(desc).positional(p).run(), vm);
  po::notify(vm);

  // Logic
  if (vm.count("help")) {
    std::cout << desc << "\n";
    std::exit(0);
  }

  return Config{
    tree, value, QT(min_str), QT(max_str),
    bsearch_depth, nthreads,
    vm.count("lb") > 0, vm.count("ub") > 0,
    json_out
  };
}

// if false, we are searching for upper bound
bool searching_lb;

bool report_incompatible(
    const SofaState &v,
    const LinearForm &val,
    const QT &lb) {
  auto res = v.is_compatible(searching_lb ? val <= lb : val >= lb);
  if (!res) {
    return true;
  } else {
    return false;
  }
}

std::shared_mutex mutex;
QT res, res_min, res_max;
int bsearch_depth;

void bsearch_worker(
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
      bar->set_label(to_json(res).asString());
    
    if (!report_incompatible(v, val, res)) {
      mutex.unlock_shared();
      mutex.lock();
      if (!report_incompatible(v, val, res)) {
        bool res_found = false;
        QT clb = res_min, cub = res_max;
        for (int i = 0; i < bsearch_depth; i++) {
          QT md = (clb + cub) / 2;
          if (report_incompatible(v, val, md)) {
            res_found = true;
            searching_lb ? clb = md : cub = md;
          } else {
            searching_lb ? cub = md : clb = md;
          }
        }
        // TODO: send this error to root thread
        if (!res_found)
          throw std::runtime_error("Lower bound invalid");
        if (searching_lb) {
          expect(res > clb);
          res = clb;
        } else {
          expect(res < cub);
          res = cub;
        }
      }
      mutex.unlock();
    }
    else
      mutex.unlock_shared();
  }
  if (bar)
    bar->finish();
}

void bsearch(
    const SofaBranchTree &tree, 
    const LinearForm &val,
    const Config &config) {
  res_min = config.bound_min;
  res_max = config.bound_max;
  bsearch_depth = config.bsearch_depth;

  const auto &nodes = tree.valid_states();

  if (config.find_lb) {
    searching_lb = true;

    std::future<void> futures[config.nthreads];
    for (int i = 0; i < config.nthreads; i++)
      futures[i] = std::async(bsearch_worker, i, config.nthreads, nodes, val);
    for (auto &f : futures)
      f.get();
    
    std::cout <<
      "Lower bound of " << config.linear_form << ": " << 
      to_json(res).asString() << std::endl;
  }

  if (config.find_ub) {
    searching_lb = false;

    std::future<void> futures[config.nthreads];
    for (int i = 0; i < config.nthreads; i++)
      futures[i] = std::async(bsearch_worker, i, config.nthreads, nodes, val);
    for (auto &f : futures)
      f.get();
    
    std::cout <<
      "Upper bound of " << config.linear_form << ": " << 
      to_json(res).asString() << std::endl;
  }
}

void run(const Config &cfg) {

  CerealReader reader(cfg.tree_file_path.c_str());
  SofaContext ctx(reader);
  SofaBranchTree tree(ctx, reader);
  reader.close();

  Parser parser(ctx);
  LinearForm val = parser.parse_expr(cfg.linear_form);

  bsearch(tree, val, cfg);

  // TODO: json output functionality
}

int main(int argc, char* argv[]) {
  try {

    Config cfg = parse_config(argc, argv);
    run(cfg);

  } catch(std::exception& e) {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  } catch(...) {
    std::cerr << "Exception of unknown type!\n";
    return 1;
  }

  return 0;
}
