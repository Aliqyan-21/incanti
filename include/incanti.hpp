#ifndef INCANTI_HPP
#define INCANTI_HPP

#include <iostream>

/*
 * Rules
 * i. Class and Namespace names in PascalCase
 * ii. others in snake_case.
 */

namespace Incanti {
class Parser {
public:
  Parser(const std::string_view &program_name = "",
         const std::string_view &program_desc = "")
      : program_name_(program_name), program_desc_(program_desc) {}

  void parse(int argc, char *argv[]) {
    if (argc > 0 && program_name_.empty()) {
      program_name_ = argv[0];
    }

    for (int i{0}; i < argc; ++i) {
      std::string arg = argv[i];

      if (arg == "-h" || arg == "--help") {
        print_help();
        exit(0);
      }
    }
  }

  void print_help() {
    if (!program_name_.empty()) {
      std::cout << "Usage: " << program_name_ << " [options]" << std::endl;
    }

    if (!program_desc_.empty()) {
      std::cout << program_desc_ << std::endl;
    }
  }

private:
  std::string_view program_name_;
  std::string_view program_desc_;
};
} // namespace Incanti

#endif //! INCANTI_HPP
