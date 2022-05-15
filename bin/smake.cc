#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <iterator>

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

    if (vm.count("angles")) {
      std::cout << "Compression level was set to " << angles << std::endl;
    } else {
      std::cout << "Compression level was not set.\n";
    }
    std::cout << "Q" << std::endl;
  } catch(std::exception& e) {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  } catch(...) {
    std::cerr << "Exception of unknown type!\n";
    return 1;
  }

  return 0;
}
