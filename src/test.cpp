#include "incanti.hpp"
#include <iomanip>
#include <iostream>

/* mostly AI generated file for showcasing and testing incanti */

int main(int argc, char *argv[]) {
  // parser creation like this, can give program name and program description
  Incanti::Parser parser("incanti-demo",
                         "Comprehensive demo of Incanti argument parser");

  // strings
  std::string input_file;
  std::string output_file;
  std::string mode;

  // numeric arguments
  int threads = 0;
  double threshold = 0.0;
  float scale = 0.0f;

  // bool flags
  bool verbose = false;
  bool debug = false;
  bool force = false;
  bool dry_run = false;

  try {
    // required argument like this
    parser.add("input", "i", &input_file).help("Input file path").required();

    // optional with default value
    parser.add("output", "o", &output_file)
        .help("Output file path")
        .default_value("output.txt");

    // custom converter (uppercase) like this
    parser.add("mode", "m", &mode)
        .help("Processing mode (fast/slow/balanced)")
        .default_value("balanced")
        .converter([](const std::string &s) {
          std::string upper = s;
          std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
          if (upper != "FAST" && upper != "SLOW" && upper != "BALANCED") {
            throw Incanti::ParseError("Mode must be fast, slow, or balanced");
          }
          return upper;
        });

    parser.add("threads", "t", &threads)
        .help("Number of worker threads")
        .default_value(4);

    // custom range validation
    parser.add("threshold", &threshold)
        .help("Confidence threshold (0.0-1.0)")
        .default_value(0.5)
        .converter([](const std::string &s) {
          double val = std::stod(s);
          if (val < 0.0 || val > 1.0) {
            throw Incanti::ParseError("Threshold must be between 0.0 and 1.0");
          }
          return val;
        });

    parser.add("scale", "s", &scale).help("Scaling factor").default_value(1.0f);

    // flags
    parser.flag("verbose", "v", &verbose).help("Enable verbose output");

    parser.flag("debug", "d", &debug).help("Enable debug mode");

    parser.flag("force", "f", &force).help("Force overwrite existing files");

    parser.flag("dry-run", "n", &dry_run)
        .help("Perform a dry run without making changes");

    // call parse to parse arguments
    parser.parse(argc, argv);

    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "PARSED ARGUMENTS" << std::endl;
    std::cout << std::string(50, '=') << std::endl;

    std::cout << "\nString Arguments:" << std::endl;
    std::cout << "  Input:       " << input_file << std::endl;
    std::cout << "  Output:      " << output_file << std::endl;
    std::cout << "  Mode:        " << mode << std::endl;

    std::cout << "\nNumeric Arguments:" << std::endl;
    std::cout << "  Threads:     " << threads << std::endl;
    std::cout << "  Threshold:   " << std::fixed << std::setprecision(2)
              << threshold << std::endl;
    std::cout << "  Scale:       " << scale << std::endl;

    std::cout << "\nFlags:" << std::endl;
    std::cout << "  Verbose:     " << (verbose ? "yes" : "no") << std::endl;
    std::cout << "  Debug:       " << (debug ? "yes" : "no") << std::endl;
    std::cout << "  Force:       " << (force ? "yes" : "no") << std::endl;
    std::cout << "  Dry Run:     " << (dry_run ? "yes" : "no") << std::endl;

    const auto &positional = parser.positional();
    if (!positional.empty()) {
      std::cout << "\nPositional Arguments:" << std::endl;
      for (size_t i = 0; i < positional.size(); ++i) {
        std::cout << "  [" << i << "] " << positional[i] << std::endl;
      }
    }

    std::cout << "\n" << std::string(50, '=') << std::endl;

    std::cout << "\nProcessing with configuration:" << std::endl;
    std::cout << "  Reading from: " << input_file << std::endl;
    std::cout << "  Writing to:   " << output_file << std::endl;
    std::cout << "  Mode:         " << mode << std::endl;
    std::cout << "  Using " << threads << " threads" << std::endl;

    if (dry_run) {
      std::cout << "\n[DRY RUN] No changes were made." << std::endl;
    } else if (verbose) {
      std::cout << "\n[VERBOSE] Processing complete!" << std::endl;
    }

  } catch (const Incanti::ParseError &e) {
    std::cerr << "\nError: " << e.what() << "\n" << std::endl;
    parser.print_help();
    return 1;
  } catch (const std::exception &e) {
    std::cerr << "\nUnexpected error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
