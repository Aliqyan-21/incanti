#include "incanti.hpp"

int main(int argc, char *argv[]) {
  Incanti::Parser p("incanti-test", "program to test incanti");
  p.parse(argc, argv);
  return 0;
}
