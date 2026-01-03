#ifndef INCANTI_HPP
#define INCANTI_HPP

#include <iostream>

/*
 * Rules
 * i. Class and Namespace names in PascalCase
 * ii. others in snake_case.
 */

class ParseError : public std::runtime_error {
public:
  explicit ParseError(const std::string &msg) : std::runtime_error(msg) {}
};

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

      /* long options, starting with '--' */
      if (arg.substr(0, 2) == "--") {
        std::string name = arg.substr(2);
        std::string value;

        // also support : "./prog --index=a1"
        size_t eq = name.find("=");
        if (eq != std::string::npos) {
          value = name.substr(eq + 1);
          name = name.substr(0, eq);
        }

        if (value.empty()) {
          if (i + 1 >= argc) {
            throw ParseError("Argument --" + name + " requires a value");
          }
          value = argv[++i];
        }
      } else if (arg[0] == '-' && arg.length() > 1 && arg[1] != '-') {
        /* short options, starting with '-' */
        std::string name = arg.substr(1);
        std::string value;
        if (i + 1 >= argc) {
          throw ParseError("Argument --" + name + " requires a value");
        }
        value = argv[++i];
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
