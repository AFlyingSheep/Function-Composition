#include <gtest/gtest.h>

#include <cmath>

#include "function_composition.hpp"
#include "projecting_function.hpp"

template <typename T>
T my_pow_expand_dimension(T base, T exp) {
  return std::pow(base, exp);
}

template <typename T>
T my_pow_fold_dimension(std::tuple<T, T> args) {
  return std::pow(std::get<0>(args), std::get<1>(args));
}

template <typename T>
T my_plus_three_expand_dimension(T t1, T t2, T t3) {
  return t1 + t2 + t3;
}

template <typename T>
T my_plus_three_fold_dimension(std::tuple<T, T, T> args) {
  return std::get<0>(args) + std::get<1>(args) + std::get<2>(args);
}

TEST(PrintMessageTest, CorrectOutput) {
  double i = 1, j = 2, k = 3;
  // test gather and spread
  auto pow_gathered = gather(my_pow_fold_dimension<double>);
  ASSERT_EQ(pow_gathered(2, 3), std::pow(2, 3));
  ASSERT_EQ(pow_gathered(i, j), std::pow(i, j));
  auto pow_spread = spread(my_pow_expand_dimension<double>);
  ASSERT_EQ(pow_spread(std::make_tuple(2, 3)), std::pow(2, 3));
  ASSERT_EQ(pow_spread(std::make_tuple(i, j)), std::pow(i, j));

  auto pow_expand_dimension = gather(spread(my_pow_expand_dimension<double>));
  ASSERT_EQ(pow_expand_dimension(2, 3), std::pow(2, 3));
  ASSERT_EQ(pow_expand_dimension(i, j), std::pow(i, j));
  auto pow_fold_dimension = spread(gather(my_pow_fold_dimension<double>));
  ASSERT_EQ(pow_fold_dimension(std::make_tuple(2, 3)), std::pow(2, 3));
  ASSERT_EQ(pow_fold_dimension(std::make_tuple(i, j)), std::pow(i, j));

  // test reverse
  auto pow_reversed = reverse(my_pow_expand_dimension<double>);
  ASSERT_EQ(pow_reversed(2, 3), std::pow(3, 2));
  ASSERT_EQ(pow_reversed(i, j), std::pow(j, i));
  auto pow_reversed_reversed =
      reverse(reverse(my_pow_expand_dimension<double>));
  ASSERT_EQ(pow_reversed_reversed(2, 3), std::pow(2, 3));
  ASSERT_EQ(pow_reversed_reversed(i, j), std::pow(i, j));

  // test partial
  auto plus_three_partial =
      partial(my_plus_three_expand_dimension<double>, 1, 2);
  ASSERT_EQ(plus_three_partial(3), 1 + 2 + 3);
  ASSERT_EQ(plus_three_partial(4), 1 + 2 + 4);
  ASSERT_EQ(plus_three_partial(k), 1 + 2 + k);
  auto plus_three_partial_var =
      partial(my_plus_three_expand_dimension<double>, i, j);
  ASSERT_EQ(plus_three_partial_var(3), i + j + 3);
  ASSERT_EQ(plus_three_partial_var(4), i + j + 4);
  ASSERT_EQ(plus_three_partial_var(k), i + j + k);

  // test rightPartial
  auto pow_expand_dimension_right_partial =
      rightPartial(my_pow_expand_dimension<double>, 2);
  ASSERT_EQ(pow_expand_dimension_right_partial(3), std::pow(3, 2));
  ASSERT_EQ(pow_expand_dimension_right_partial(i), std::pow(i, 2));

  auto pow_expand_dimension_right_partial_var =
      rightPartial(my_pow_expand_dimension<double>, i);
  ASSERT_EQ(pow_expand_dimension_right_partial_var(3), std::pow(3, i));
  ASSERT_EQ(pow_expand_dimension_right_partial_var(j), std::pow(j, i));

  // test curry
  auto pow_curried = curry(my_pow_expand_dimension<double>);
  ASSERT_EQ(pow_curried(2)(3), std::pow(2, 3));
  ASSERT_EQ(pow_curried(i)(j), std::pow(i, j));
}

int main() {
  ::testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}
