#ifndef INCANTI_HPP
#define INCANTI_HPP

#include <iostream>

/*
 * Rules
 * i. Class and Namespace names in Pascal Case
 * ii. others in snake case.
 */

namespace Incanti {
class Parser {
public:
  void print_help() { std::cout << "Hello, I am Incanti" << std::endl; }

private:
};
} // namespace Incanti

#endif //! INCANTI_HPP
