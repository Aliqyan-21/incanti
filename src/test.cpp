#include "incanti.hpp"

int main(int argc, char *argv[]) {
  Incanti::Parser p("incanti-test", "program to test incanti");
  bool debug = false;
  p.flag("debug", "d", &debug);
  p.parse(argc, argv);
  std::cerr << (debug ? "true" : "false") << std::endl;
  return 0;
}
