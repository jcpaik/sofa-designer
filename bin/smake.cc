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
#include "csv.h"

static bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && 0 == 
      str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

void process_angles(const std::vector<std::vector<std::string> > &csv, 
                    const std::string &name) {
  std::vector<Vector> gamma;
  if (csv[0] != std::vector<std::string>({"branching order", "x", "y", "r"})) {
    throw std::invalid_argument("CSV header");
  }

  std::vector<int> bidx;
  for (size_t i = 1; i < csv.size(); i++) {
    NT x(csv[i][1]), y(csv[i][2]), r(csv[i][3]);
    gamma.emplace_back(QT(x, r), QT(y, r));

    auto index = csv[i][0];
    if (index.size()) {
      int ii = stoi(index);
      if (ii < 0) {
        throw std::invalid_argument("Negative index");
      }
      if (bidx.size() <= ii) {
        bidx.resize(ii + 1, -1);
      }
      bidx[ii] = i;
    }
  }

  // TODO: make this prettier
  for (auto g : gamma)
    std::cout << g.x << " " << g.y << std::endl;

  SofaContext ctx(gamma);
  save((name + ".ctx").c_str(), ctx);

  SofaBranchTree t(ctx, bidx[0]);
  for (int i = 1; i < bidx.size(); i++) {
    t.add_corner(bidx[i]);
  }
  save((name + ".sofas").c_str(), t);
}

int main(int argc, char* argv[]) {
  try {
    std::string angles;
    unsigned int nthreads;

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

    const std::string ext(".angles");
    if (!vm.count("angles")) {
      throw std::invalid_argument("Angles missing");
    } else if (!endsWith(angles, ext)) {
      throw std::invalid_argument(
          "The file for angles should have .angles extension\n");
    }

    std::string name = angles.substr(0, angles.size() - ext.size());
    std::ifstream inp(angles);
    auto gamma_csv = readCSV(inp);
    process_angles(gamma_csv, name);
  } catch(std::exception& e) {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  } catch(...) {
    std::cerr << "Exception of unknown type!\n";
    return 1;
  }

  return 0;
}
