#include <gtest/gtest.h>

#include "function_composition.hpp"
#include "projecting_function.hpp"

void function1(int i, int j) { printf("%d %d\n", i, j); }

void function2(std::tuple<int, int> t) {
  printf("%d %d\n", std::get<0>(t), std::get<1>(t));
}

void function3(int i, double j) { printf("%d %.4lf\n", i, j); }

void function4(std::tuple<int, double> t) {
  printf("%d %.4lf\n", std::get<0>(t), std::get<1>(t));
}

void printMessage() { std::cout << "Hello, World!" << std::endl; }

TEST(PrintMessageTest, CorrectOutput) {
  // capture the output
  testing::internal::CaptureStdout();
  std::string output;
  auto spreadFunction = spread(function1);
  spreadFunction(std::make_tuple(1, 2));
  output = testing::internal::GetCapturedStdout();
  ASSERT_EQ(output, "1 2\n");

  testing::internal::CaptureStdout();
  auto gatherFunction = gather(function2);
  gatherFunction(1, 2);
  output = testing::internal::GetCapturedStdout();
  ASSERT_EQ(output, "1 2\n");

  testing::internal::CaptureStdout();
  auto still_function = gather(spread(function1));
  still_function(1, 2);
  output = testing::internal::GetCapturedStdout();
  ASSERT_EQ(output, "1 2\n");

  testing::internal::CaptureStdout();
  auto still_fcuntion2 = spread(gather(function2));
  still_fcuntion2(std::make_tuple(1, 2));
  output = testing::internal::GetCapturedStdout();
  ASSERT_EQ(output, "1 2\n");

  testing::internal::CaptureStdout();
  auto spreadFunction2 = spread(function3);
  spreadFunction2(std::make_tuple(1, 2.0));
  output = testing::internal::GetCapturedStdout();
  ASSERT_EQ(output, "1 2.0000\n");

  testing::internal::CaptureStdout();
  auto gatherFunction2 = gather(function4);
  gatherFunction2(1, 2.0);
  output = testing::internal::GetCapturedStdout();
  ASSERT_EQ(output, "1 2.0000\n");

  testing::internal::CaptureStdout();
  auto still_function2 = gather(spread(function3));
  still_function2(1, 2.0);
  output = testing::internal::GetCapturedStdout();
  ASSERT_EQ(output, "1 2.0000\n");

  testing::internal::CaptureStdout();
  auto still_fcuntion3 = spread(gather(function4));
  still_fcuntion3(std::make_tuple(1, 2.0));
  output = testing::internal::GetCapturedStdout();
  ASSERT_EQ(output, "1 2.0000\n");
}

int main() {
  ::testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}
