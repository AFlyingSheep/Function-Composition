cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

project(FunctionComposition)

include_directories(${CMAKE_SOURCE_DIR}/include)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 导出 compile_commands.json 文件
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(test)
