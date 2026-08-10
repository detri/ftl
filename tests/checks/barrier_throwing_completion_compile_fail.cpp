#include <barrier>

struct throwing_completion {
  void operator()() {}
};

std::barrier<throwing_completion> invalid_barrier(
    1, throwing_completion{});

int main() {}
