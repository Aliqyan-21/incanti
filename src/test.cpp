#include "incanti.hpp"
#include <cstdint>

int main(int argc, char *argv[]) {
  Incanti::Parser p("incanti-test", "program to test incanti");
  float height, width;
  uint32_t point;

  p.add("height", "y", &height).required();
  p.add("width", "x", &width).default_value(0.0f).is_required();
  // p.add("point", "p", &point).default_value(0);

  p.add("point", "p", &point)
      .default_value(0)
      .converter([](const std::string &s) {
        uint32_t val = stoul(s);
        return val;
      });

  p.parse(argc, argv);

  std::cout << "height: " << height << std::endl;
  std::cout << "width: " << width << std::endl;
  std::cout << "point: " << point << std::endl;
  return 0;
}
