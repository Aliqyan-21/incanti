#include "incanti.hpp"

int main(int argc, char *argv[]) {
  Incanti::Parser p;
  float scale;

  try {
    p.add("scale", "s", &scale)
        .help("scale image with this multiplier")
        .required();
    p.parse(argc, argv);
    std::cout << "scaling the image to: " << scale << "x" << std::endl;
  } catch (const Incanti::ParseError &pe) {
    p.print_help();
  }

  return 0;
}
