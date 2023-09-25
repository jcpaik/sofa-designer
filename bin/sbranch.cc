#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <iterator>
#include <istream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <utility>

#include "sofa/context.h"
#include "sofa/geom.h"
#include "sofa/branch_tree.h"

static bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && 0 == 
      str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

QT rational(const std::string &s) {
  std::stringstream sin(s);
  QT res;
  sin >> res;
  if (!sin.eof())
    throw std::invalid_argument(
      std::string("Invalid rational number: ") + s);
  return res;
}

void process_angles(Json::Value &angles, unsigned int nthreads) {
  if (angles.type() != Json::arrayValue)
    throw std::invalid_argument("JSON not an array");
  
  std::vector< std::pair<int, int> > order_pair;

  for (int i = 0; i < angles.size(); i++) {
    const auto &angle = angles[i];
    int order = angle["branch_order"].asInt();
    if (order < 0)
      continue;
    order_pair.emplace_back(order, i + 1);
  }

  // Determine the corner indices in branching order
  std::sort(order_pair.begin(), order_pair.end());
  std::vector<int> bidx(order_pair.size());
  for (size_t i = 0; i < order_pair.size(); i++)
    bidx[i] = order_pair[i].second;

  // Branching
  SofaContext ctx(angles);
  SofaBranchTree t(ctx);
  for (int i = 0; i < bidx.size(); i++) {
    t.add_corner(bidx[i], true, nthreads);
  }

  // Print relevant information
  QT marea(0);
  auto x(t.valid_states());
  for (auto &s : x) {
    auto a = s.area();
    marea = std::max(marea, a);
  }
  std::cout << "Number of valid states: " << t.valid_states().size() << std::endl;
  std::cout << "Area: " << marea << std::endl;
}

int main(int argc, char* argv[]) {
  try {
    std::string angles;
    unsigned int nthreads = 1;

    // Set up syntax for arguments
    po::options_description desc("Allowed options");
    desc.add_options()
      ("help", "Produce help message")
      ("angles", po::value<std::string>(&angles), "Required angle partition")
      ("nthreads", po::value<unsigned int>(&nthreads)->implicit_value(1),
         "Number of threads to use (optional)\n"
         "Note that the output is not deterministic "
         "when the option is specified")
      ;

    po::positional_options_description p;
    p.add("angles", -1);

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

    const std::string ext(".json");
    if (!vm.count("angles")) {
      throw std::invalid_argument("Angles missing");
    } else if (!endsWith(angles, ext)) {
      throw std::invalid_argument(
          "The file for angles should have .json extension\n");
    }

    std::ifstream inp(angles);
    Json::Value angles_json;
    inp >> angles_json;
    process_angles(angles_json, nthreads);
  } catch(std::exception& e) {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  } catch(...) {
    std::cerr << "Exception of unknown type!\n";
    return 1;
  }

  return 0;
}
