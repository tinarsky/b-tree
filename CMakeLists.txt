cmake_minimum_required(VERSION 3.23)
project(b_tree)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(b_tree main.cpp b_tree.h)

add_executable(b_tree_test b_tree_test.cc)
target_link_libraries(
        b_tree_test
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(b_tree_test)
