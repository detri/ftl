// Freestanding Template Library
// SPDX-License-Identifier: MIT
#ifndef FTL_ALGORITHMS_RESULT_HEADER
#define FTL_ALGORITHMS_RESULT_HEADER

#include <concepts>
#include <type_traits>
#include <utility>


namespace std {

namespace ranges {
template <class Input, class Func> struct in_fun_result {
  FTL_NO_UNIQUE_ADDRESS Input in;
  FTL_NO_UNIQUE_ADDRESS Func fun;

  template <class InputOther, class FuncOther>
    requires convertible_to<const Input &, InputOther> &&
             convertible_to<const Func &, FuncOther>
  constexpr operator in_fun_result<InputOther, FuncOther>() const & {
    return {in, fun};
  }

  template <class InputOther, class FuncOther>
    requires convertible_to<Input, InputOther> &&
             convertible_to<Func, FuncOther>
  constexpr operator in_fun_result<InputOther, FuncOther>() && {
    return {std::move(in), std::move(fun)};
  }
};

template <class Input1, class Input2> struct in_in_result {
  FTL_NO_UNIQUE_ADDRESS Input1 in1;
  FTL_NO_UNIQUE_ADDRESS Input2 in2;

  template <class Input1Other, class Input2Other>
    requires convertible_to<const Input1 &, Input1Other> &&
             convertible_to<const Input2 &, Input2Other>
  constexpr operator in_in_result<Input1Other, Input2Other>() const & {
    return {in1, in2};
  }

  template <class Input1Other, class Input2Other>
    requires convertible_to<Input1, Input1Other> &&
             convertible_to<Input2, Input2Other>
  constexpr operator in_in_result<Input1Other, Input2Other>() && {
    return {std::move(in1), std::move(in2)};
  }
};

template <class Input, class Output> struct in_out_result {
  FTL_NO_UNIQUE_ADDRESS Input in;
  FTL_NO_UNIQUE_ADDRESS Output out;

  template <class InputOther, class OutputOther>
    requires convertible_to<const Input &, InputOther> &&
             convertible_to<const Output &, OutputOther>
  constexpr operator in_out_result<InputOther, OutputOther>() const & {
    return {in, out};
  }

  template <class InputOther, class OutputOther>
    requires convertible_to<Input, InputOther> &&
             convertible_to<Output, OutputOther>
  constexpr operator in_out_result<InputOther, OutputOther>() && {
    return {std::move(in), std::move(out)};
  }
};

template <class Input1, class Input2, class Output> struct in_in_out_result {
  FTL_NO_UNIQUE_ADDRESS Input1 in1;
  FTL_NO_UNIQUE_ADDRESS Input2 in2;
  FTL_NO_UNIQUE_ADDRESS Output out;

  template <class Input1Other, class Input2Other, class OutputOther>
    requires convertible_to<const Input1 &, Input1Other> &&
             convertible_to<const Input2 &, Input2Other> &&
             convertible_to<const Output &, OutputOther>
  constexpr
  operator in_in_out_result<Input1Other, Input2Other, OutputOther>() const & {
    return {in1, in2, out};
  }

  template <class Input1Other, class Input2Other, class OutputOther>
    requires convertible_to<Input1, Input1Other> &&
             convertible_to<Input2, Input2Other> &&
             convertible_to<Output, OutputOther>
  constexpr
  operator in_in_out_result<Input1Other, Input2Other, OutputOther>() && {
    return {std::move(in1), std::move(in2), std::move(out)};
  }
};

template <class Input, class Output1, class Output2> struct in_out_out_result {
  FTL_NO_UNIQUE_ADDRESS Input in;
  FTL_NO_UNIQUE_ADDRESS Output1 out1;
  FTL_NO_UNIQUE_ADDRESS Output2 out2;

  template <class InputOther, class Output1Other, class Output2Other>
    requires convertible_to<const Input &, InputOther> &&
             convertible_to<const Output1 &, Output1Other> &&
             convertible_to<const Output2 &, Output2Other>
  constexpr
  operator in_out_out_result<InputOther, Output1Other, Output2Other>() const & {
    return {in, out1, out2};
  }

  template <class InputOther, class Output1Other, class Output2Other>
    requires convertible_to<Input, InputOther> &&
             convertible_to<Output1, Output1Other> &&
             convertible_to<Output2, Output2Other>
  constexpr
  operator in_out_out_result<InputOther, Output1Other, Output2Other>() && {
    return {std::move(in), std::move(out1), std::move(out2)};
  }
};

template <class T> struct min_max_result {
  FTL_NO_UNIQUE_ADDRESS T min;
  FTL_NO_UNIQUE_ADDRESS T max;

  template <class Other>
    requires convertible_to<const T &, Other>
  constexpr operator min_max_result<Other>() const & {
    return {min, max};
  }

  template <class Other>
    requires convertible_to<T, Other>
  constexpr operator min_max_result<Other>() && {
    return {std::move(min), std::move(max)};
  }
};

template <class T> using minmax_result = min_max_result<T>;

template <class T> struct in_found_result {
  FTL_NO_UNIQUE_ADDRESS T in;
  bool found;

  template <class Other>
    requires convertible_to<const T &, Other>
  constexpr operator in_found_result<Other>() const & {
    return {in, found};
  }

  template <class Other>
    requires convertible_to<T, Other>
  constexpr operator in_found_result<Other>() && {
    return {std::move(in), found};
  }
};

template <class Input, class T> struct in_value_result {
  FTL_NO_UNIQUE_ADDRESS Input in;
  FTL_NO_UNIQUE_ADDRESS T value;

  template <class InputOther, class Other>
    requires convertible_to<const Input&, InputOther> && convertible_to<const T&, Other>
  constexpr operator in_value_result<InputOther, Other>() const & {
    return {in, value};
  }

  template <class InputOther, class Other>
    requires convertible_to<Input, InputOther> && convertible_to<T, Other>
  constexpr operator in_value_result<InputOther, Other>() && {
    return {std::move(in), std::move(value)};
  }
};

template <class Output, class T> struct out_value_result {
  FTL_NO_UNIQUE_ADDRESS Output out;
  FTL_NO_UNIQUE_ADDRESS T value;

  template <class OutputOther, class Other>
    requires convertible_to<const Output&, OutputOther> && convertible_to<const T&, Other>
  constexpr operator out_value_result<OutputOther, Other>() const & {
    return {out, value};
  }

  template <class OutputOther, class Other>
    requires convertible_to<Output, OutputOther> && convertible_to<T, Other>
  constexpr operator out_value_result<OutputOther, Other>() && {
    return {std::move(out), std::move(value)};
  }
};

} // namespace ranges

} // namespace std


#endif // FTL_ALGORITHMS_RESULT_HEADER
