#include <random>

extern "C" int ftl_entry() {
  std::minstd_rand engine;
  return engine() == 48271 ? 0 : 1;
}
