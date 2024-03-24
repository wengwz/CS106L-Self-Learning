
cmake_minimum_required(VERSION 3.14)
project(hashmap)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_BUILD_TYPE "Debug")

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -Wall")

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  hashmap_test
  hashmap_test.cpp
)
target_link_libraries(
  hashmap_test
  GTest::gtest_main
)

add_executable(
    hashmap_perf
    hashmap_perf.cpp
)

target_link_libraries(
  hashmap_perf
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(hashmap_test)