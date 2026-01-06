#ifndef INCANTI_HPP
#define INCANTI_HPP

#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>

/*
 * Rules
 * i. Class and Namespace names in PascalCase
 * ii. others in snake_case.
 */

namespace Incanti {
class ParseError : public std::runtime_error {
public:
  explicit ParseError(const std::string &msg) : std::runtime_error(msg) {}
};

struct required_t {
} constexpr required{};

struct help_t {};

struct default_t {};
template <typename T> struct default_wrapper {
  T value;
  constexpr default_wrapper(T v) : value(std::move(v)) {}
};

struct converter_t {};

template <typename T> struct ArgP {
  std::string name;
  std::string short_name;
  T *value_ptr;

  ArgP(std::string n, std::string sn, T *p)
      : name(std::move(n)), short_name(std::move(sn)), value_ptr(p) {}
};

struct FlagP {
  std::string name;
  std::string short_name;
  bool *value_ptr;

  FlagP(std::string n, std::string sn, bool *p)
      : name(std::move(n)), short_name(std::move(sn)), value_ptr(p) {}
};

class Argument {
public:
  virtual ~Argument() = default;
  virtual void parse(const std::string &value) = 0;
  virtual bool has_value() const = 0;
  virtual std::string get_help() const = 0;
  virtual bool is_required() const = 0;
  virtual std::string get_name() const = 0;
};

template <typename T> class TypedArgument : public Argument {
public:
  TypedArgument(const std::string &name, const std::string &short_name,
                T *value_ptr)
      : name_(name), short_name_(short_name), value_ptr_(value_ptr),
        required_(false), has_default_(false), parsed_(false) {
    str_to_T_ = [this](const std::string &s) { return converter_(s); };
  }

  void parse(const std::string &value) override {
    try {
      *value_ptr_ = str_to_T_(value);
      parsed_ = true;
    } catch (const std::exception &e) {
      throw ParseError("Failed to parse '" + value + "' for argument --" +
                       name_ + ": " + e.what());
    }
  }
  bool has_value() const override { return parsed_ || has_default_; }
  bool is_required() const override { return required_; }
  std::string get_name() const override { return name_; }

  std::string get_help() const override {
    std::string result;
    if (!short_name_.empty()) {
      result += "-" + short_name_ + ", ";
    }
    result += "--" + name_;

    if (!std::is_same_v<T, bool>) {
      result += " <value>";
    }

    if (!help_.empty()) {
      result += "\n   " + help_;
    }

    if (has_default_ && !required_) {
      std::ostringstream oss;
      oss << default_val_;
      result += " (default: " + oss.str() + ")";
    }

    if (required_) {
      result += " [required]";
    }

    return result;
  }

  TypedArgument &help(const std::string &help_text) {
    help_ = help_text;
    return *this;
  }

  TypedArgument &default_value(const T &value) {
    default_val_ = value;
    has_default_ = true;
    if (!parsed_) {
      *value_ptr_ = value;
    }
    return *this;
  }

  TypedArgument &required() {
    required_ = true;
    return *this;
  }

  TypedArgument &converter(std::function<T(const std::string &)> conv) {
    str_to_T_ = conv;
    return *this;
  }

  TypedArgument<T> &operator|(const char *help_text) {
    help_ = help_text;
    return *this;
  }

  TypedArgument<T> &operator|(const std::string &help_text) {
    help_ = help_text;
    return *this;
  }

  TypedArgument<T> &operator|(required_t) {
    required_ = true;
    return *this;
  }

  template <typename U>
  TypedArgument<T> &operator|(const default_wrapper<U> &def_val) {
    static_assert(std::is_convertible_v<U, T>,
                  "Default value must be convertible to argument type");
    default_val_ = static_cast<T>(def_val.value);
    has_default_ = true;
    if (!parsed_) {
      *value_ptr_ = default_val_;
    }
    return *this;
  }

  template <typename Func, typename = std::enable_if_t<std::is_invocable_r_v<
                               T, Func, const std::string &>>>
  TypedArgument<T> &operator|(Func &&converter) {
    str_to_T_ = std::forward<Func>(converter);
    return *this;
  }

private:
  std::string name_;
  std::string short_name_;
  std::string help_;
  T *value_ptr_;
  T default_val_;
  bool required_;
  bool has_default_;
  bool parsed_;
  std::function<T(const std::string &value)> str_to_T_;

  bool boolify_(const std::string &str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower == "true" || lower == "1" || lower == "yes")
      return true;
    if (lower == "false" || lower == "0" || lower == "no")
      return false;
    throw ParseError("Invalid boolean value: " + str);
  }

  T converter_(const std::string &str) {
    if constexpr (std::is_same_v<T, std::string>) {
      return str;
    } else if constexpr (std::is_same_v<T, int>) {
      return std::stoi(str);
    } else if constexpr (std::is_same_v<T, float>) {
      return std::stof(str);
    } else if constexpr (std::is_same_v<T, double>) {
      return std::stod(str);
    } else if constexpr (std::is_same_v<T, long>) {
      return std::stol(str);
    } else if constexpr (std::is_same_v<T, bool>) {
      return boolify_(str);
    } else {
      // todo: add link to docs.
      throw ParseError("No default converter for this type. "
                       "Please provide a custom .converter() for --" +
                       name_);
    }
  }
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

  FlagArgument &operator|(const char *help_text) {
    help_ = help_text;
    return *this;
  }

  FlagArgument &operator|(const std::string &help_text) {
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
  Parser(const std::string &program_name = "",
         const std::string &program_desc = "")
      : program_name_(program_name), program_desc_(program_desc),
        help_added_(false) {
    add_help_flag();
  }

  template <typename T>
  TypedArgument<T> &arg(const std::string &name, const std::string &short_name,
                        T *value_ptr) {
    if (arguments_.find(name) != arguments_.end()) {
      throw ParseError("Duplicate argument name: --" + name);
    }

    if (!short_name.empty() &&
        short_to_long_.find(short_name) != short_to_long_.end()) {
      throw ParseError("Duplicate short option name: " + short_name +
                       " (already used by --" + short_to_long_[short_name] +
                       ")");
    }

    auto arg = std::make_shared<TypedArgument<T>>(name, short_name, value_ptr);
    arguments_[name] = arg;
    if (!short_name.empty()) {
      short_to_long_[short_name] = name;
    }
    return *arg;
  }

  template <typename T>
  TypedArgument<T> &arg(const std::string &name, T *value_ptr) {
    return arg(name, "", value_ptr);
  }

  /* add args which are flags - true/false */
  FlagArgument &flag(const std::string &name, const std::string &short_name,
                     bool *value_ptr) {
    if (arguments_.find(name) != arguments_.end()) {
      throw ParseError("Duplicate argument name: --" + name);
    }

    if (!short_name.empty() &&
        short_to_long_.find(short_name) != short_to_long_.end()) {
      throw ParseError("Duplicate short option name: " + short_name +
                       " (already used by --" + short_to_long_[short_name] +
                       ")");
    }

    auto arg = std::make_shared<FlagArgument>(name, short_name, value_ptr);
    arguments_[name] = arg;
    if (!short_name.empty()) {
      short_to_long_[short_name] = name;
    }
    return *arg;
  }

  /* add args which are flags - true/false (when no short name) */
  FlagArgument &flag(const std::string &name, bool *value_ptr) {
    return flag(name, "", value_ptr);
  }

  void parse(int argc, char *argv[]) {
    if (argc > 0 && program_name_.empty()) {
      program_name_ = argv[0];
    }

    for (int i{1}; i < argc; ++i) {
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
        std::string short_name = arg.substr(1);

        auto match = short_to_long_.find(short_name);
        if (match != short_to_long_.end()) {
          auto arg_it = arguments_.find(match->second);
          auto flag_arg = dynamic_cast<FlagArgument *>(arg_it->second.get());

          if (flag_arg) {
            flag_arg->parse("");
          } else {
            if (i + 1 >= argc) {
              throw ParseError("Argument --" + short_name +
                               " requires a value");
            }
            arg_it->second->parse(argv[++i]);
          }
        } else {
          /* combining of different flags like : '-vdi' */
          for (size_t j{0}; j < short_name.length(); ++j) {
            std::string short_opt(1, short_name[j]);
            auto it = short_to_long_.find(short_opt);
            if (it == short_to_long_.end()) {
              throw ParseError("Unknown argument: -" + short_opt);
            }

            auto arg_it = arguments_.find(it->second);
            auto flag_arg = dynamic_cast<FlagArgument *>(arg_it->second.get());

            if (flag_arg) {
              flag_arg->parse("");
            } else {
              if (j < short_name.length() - 1) {
                // value is attached: -ofile.txt | -vfd (combinatorics magic)
                arg_it->second->parse(short_name.substr(j + 1));
                break;
              } else if (i + 1 >= argc) {
                throw ParseError("Argument -" + short_opt +
                                 " requires a value");
              } else {
                arg_it->second->parse(argv[++i]);
              }
            }
          }
        }
      } else {
        positionals_.push_back(arg);
      }
    }

    for (const auto &[name, arg] : arguments_) {
      if (arg->is_required() && !arg->has_value()) {
        throw ParseError("Required argument missing: --" + name);
      }
    }
  }

  const std::vector<std::string> &positional() const { return positionals_; }

  void print_help() {
    if (!program_name_.empty()) {
      std::cout << "Usage: " << program_name_ << " [options]" << std::endl;
    }

    if (!program_desc_.empty()) {
      std::cout << program_desc_ << std::endl;
    }

    std::cout << "\nOptions: " << std::endl;
    for (const auto &[name, arg] : arguments_) {
      std::cout << arg->get_help() << std::endl;
    }
  }

private:
  std::string program_name_;
  std::string program_desc_;
  std::map<std::string, std::shared_ptr<Argument>> arguments_;
  std::map<std::string, std::string> short_to_long_;
  std::vector<std::string> positionals_;
  bool help_added_;

  void add_help_flag() {
    if (!help_added_) {
      auto help_flag =
          std::make_shared<FlagArgument>("help", "h", new bool(false));
      help_flag->help("Show this help message");
      arguments_["help"] = help_flag;
      short_to_long_["h"] = "help";
      help_added_ = true;
    }
  }
};
} // namespace Incanti

inline constexpr Incanti::required_t required{};

template <typename T> constexpr auto def(T &&value) {
  return Incanti::default_wrapper<std::decay_t<T>>{std::forward<T>(value)};
}

template <typename T>
Incanti::ArgP<T> arg(std::string name, std::string short_name, T *value_ptr) {
  return Incanti::ArgP<T>{std::move(name), std::move(short_name), value_ptr};
}
template <typename T> Incanti::ArgP<T> arg(std::string name, T *value_ptr) {
  return arg(std::move(name), "", value_ptr);
}

inline Incanti::FlagP flag(std::string name, std::string short_name,
                           bool *value_ptr) {
  return Incanti::FlagP{std::move(name), std::move(short_name), value_ptr};
}
inline Incanti::FlagP flag(std::string name, bool *value_ptr) {
  return Incanti::FlagP{std::move(name), "", value_ptr};
}

template <typename T>
Incanti::TypedArgument<T> &operator>>(Incanti::Parser &parser,
                                      Incanti::ArgP<T> p) {
  return parser.arg(std::move(p.name), std::move(p.short_name), p.value_ptr);
}

inline Incanti::FlagArgument &operator>>(Incanti::Parser &parser,
                                         Incanti::FlagP p) {
  return parser.flag(std::move(p.name), std::move(p.short_name), p.value_ptr);
}

#endif //! INCANTI_HPP
