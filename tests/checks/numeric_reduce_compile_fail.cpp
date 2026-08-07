#include <numeric>

struct asymmetric_reduce {
  constexpr long operator()(long left, int right) const { return left + right; }

  constexpr long operator()(int, long) const = delete;

  constexpr long operator()(long left, long right) const {
    return left + right;
  }

  constexpr long operator()(int left, int right) const { return left + right; }
};

int main() {
  int values[]{1, 2, 3};

  (void)std::reduce(values, values + 3, 0L, asymmetric_reduce{});
}
