enable_testing()

add_executable(test_args_convert args_convert_test/test.cc)
target_link_libraries(test_args_convert gtest_main)
