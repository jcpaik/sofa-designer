#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <iterator>
#include <istream>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>

#include "csv.h"

static bool endsWith(const std::string& str, const std::string& suffix)
{
    return str.size() >= suffix.size() && 0 == 
      str.compare(str.size()-suffix.size(), suffix.size(), suffix);
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
    std::cout << "The file has name " << name << std::endl;

    std::ifstream inp(angles);
    auto iarr = readCSV(inp);

    std::cout << iarr.size() << std::endl;

  } catch(std::exception& e) {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  } catch(...) {
    std::cerr << "Exception of unknown type!\n";
    return 1;
  }

  return 0;
}
