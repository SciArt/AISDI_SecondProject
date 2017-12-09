#include <cstddef>
#include <cstdlib>
#include <string>

#include "TreeMap.h"

namespace
{

template <typename K, typename V>
using Map = aisdi::Map<K, V>;

void perfomTest()
{
  Map<int, std::string> map;
  map[1] = "TODO";
}

} // namespace

int main(int argc, char** argv)
{
  const std::size_t repeatCount = argc > 1 ? std::atoll(argv[1]) : 10000;
  for (std::size_t i = 0; i < repeatCount; ++i)
    perfomTest();
  return 0;
}
