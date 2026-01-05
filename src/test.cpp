#include "incanti.hpp"
#include <cstdint>

int main(int argc, char *argv[]) {
  Incanti::Parser p("incanti-test", "program to test incanti");
  std::string input_file;

  bool verbose = false;
  bool debug = false;
  bool force = false;
  bool dry_run = false;

  p.add("input", "i", &input_file).help("Input file path").required();

  p.flag("verbose", "v", &verbose).help("Enable verbose output");

  p.flag("debug", "d", &debug).help("Enable debug mode");

  p.flag("force", "f", &force).help("Force overwrite existing files");

  p.flag("dry-run", "n", &dry_run)
      .help("Perform a dry run without making changes");

  p.parse(argc, argv);

  std::cout << "input: " << input_file << std::endl;

  std::cout << "verbose: " << verbose << std::endl;
  std::cout << "force: " << force << std::endl;
  std::cout << "debug: " << debug << std::endl;
  std::cout << "dry run: " << dry_run << std::endl;
  return 0;
}
