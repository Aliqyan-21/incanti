#ifndef INCANTI_HPP
#define INCANTI_HPP

#include <iostream>
#include <map>
#include <memory>

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
class Argument {
public:
  virtual ~Argument() = default;
  virtual void parse(const std::string &value) = 0;
  virtual bool has_value() const = 0;
  virtual std::string get_help() const = 0;
  virtual bool is_required() const = 0;
  virtual std::string get_name() const = 0;
};

class FlagArgument : public Argument {
public:
  FlagArgument(const std::string &name, const std::string &short_name,
               bool *value_ptr)
      : name_(name), short_name_(short_name), value_ptr_(value_ptr),
        parsed_(false) {
    *value_ptr_ = false;
  }

  void parse(const std::string &value) override {
    *value_ptr_ = true;
    parsed_ = true;
  }

  bool has_value() const override { return true; }
  bool is_required() const override { return false; }

  std::string get_name() const override { return name_; }

  std::string get_help() const override {
    std::string result;
    if (short_name_.empty()) {
      result += "-" + short_name_ + ", ";
    }
    result += "--" + name_;

    if (!help_.empty()) {
      result += "\n   " + help_;
    }

    return result;
  }

  FlagArgument &help(const std::string &help_text) {
    help_ = help_text;
    return *this;
  }

private:
  std::string name_;
  std::string short_name_;
  std::string help_;
  bool *value_ptr_;
  bool parsed_;
};

class Parser {
public:
  Parser(const std::string_view &program_name = "",
         const std::string_view &program_desc = "")
      : program_name_(program_name), program_desc_(program_desc) {}

  FlagArgument &flag(const std::string &name, const std::string &short_name,
                     bool *value_ptr) {
    auto arg = std::make_shared<FlagArgument>(name, short_name, value_ptr);
    arguments_[name] = arg;
    return *arg;
  }

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

        auto it = arguments_.find(name);
        if (it == arguments_.end()) {
          throw ParseError("Unknown Argument: --" + name);
        }

        auto flag_arg = dynamic_cast<FlagArgument *>(it->second.get());
        if (flag_arg) {
          flag_arg->parse("");
        } else {
          if (value.empty()) {
            if (i + 1 >= argc) {
              throw ParseError("Argument --" + name + " requires a value");
            }
            value = argv[++i];
          }
          it->second->parse(value);
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
  std::map<std::string, std::shared_ptr<Argument>> arguments_;
};
} // namespace Incanti

#endif //! INCANTI_HPP
