cmake_minimum_required(VERSION 3.20)
project(sorting_seminars LANGUAGES CXX)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Project/library containing all sorting algorithms from seminars 1-4.
add_library(sorting STATIC
    src/insertion_sort.cpp
    src/selection_sort.cpp
    src/exchange_sort.cpp
    src/bubble_sort.cpp
    src/optimized_bubble_sort.cpp
    src/binary_insertion_sort.cpp
    src/shaker_sort.cpp
    src/shell_sort.cpp
    src/quick_sort.cpp
    src/two_phase_merge_sort.cpp
    src/one_phase_merge_sort.cpp
    src/natural_merge_sort.cpp
)
target_include_directories(sorting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(sorting PUBLIC cxx_std_17)

include(CTest)
if(BUILD_TESTING)
    include(FetchContent)
    set(gtest_force_shared_crt ON CACHE BOOL "Use shared MSVC runtime" FORCE)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    )
    FetchContent_MakeAvailable(googletest)

    # One test project; the parameterized suite applies the same cases to every algorithm.
    add_executable(sorting_tests tests/test_all_sorts.cpp)
    target_link_libraries(sorting_tests PRIVATE sorting GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(sorting_tests)
endif()
