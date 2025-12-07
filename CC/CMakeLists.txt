cmake_minimum_required(VERSION 3.14)
project(dcqcn LANGUAGES CXX)

# --- C++ setup ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Your library (DCQCN + Scheduler) ---
add_library(dcqcn
    DCQCN.cpp
    Scheduler.cpp
)
target_include_directories(dcqcn PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# --- GoogleTest via FetchContent (no system install needed) ---
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
# On MSVC, keep runtime consistent (optional)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
enable_testing()

# --- Test binary ---
add_executable(dcqcn_tests
    tests/test_dcqcn_cnp.cpp
)
target_link_libraries(dcqcn_tests
    PRIVATE
        dcqcn
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(dcqcn_tests)

