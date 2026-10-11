#include <gtest/gtest.h>

#include <algorithm>
#include <string>
#include <vector>

#include "binary_insertion_sort.h"
#include "bubble_sort.h"
#include "exchange_sort.h"
#include "insertion_sort.h"
#include "natural_merge_sort.h"
#include "one_phase_merge_sort.h"
#include "optimized_bubble_sort.h"
#include "quick_sort.h"
#include "selection_sort.h"
#include "shaker_sort.h"
#include "shell_sort.h"
#include "two_phase_merge_sort.h"

namespace {

using IntSortFunction = void (*)(std::vector<int>&);
using DoubleSortFunction = void (*)(std::vector<double>&);

struct IntAlgorithm {
    const char* name;
    IntSortFunction sort;
};

struct DoubleAlgorithm {
    const char* name;
    DoubleSortFunction sort;
};

class IntSortingTest : public ::testing::TestWithParam<IntAlgorithm> {};
class DoubleSortingTest : public ::testing::TestWithParam<DoubleAlgorithm> {};

void expect_int_result(IntSortFunction sort,
                       std::vector<int> actual,
                       const std::vector<int>& expected) {
    sort(actual);
    EXPECT_EQ(actual, expected);
}

void expect_double_result(DoubleSortFunction sort,
                          std::vector<double> actual,
                          const std::vector<double>& expected) {
    sort(actual);
    EXPECT_EQ(actual, expected);
}

#define INT_ALGORITHM(name) IntAlgorithm{#name, &sorting::name<int>}
#define DOUBLE_ALGORITHM(name) DoubleAlgorithm{#name, &sorting::name<double>}

INSTANTIATE_TEST_SUITE_P(
    AllAlgorithms,
    IntSortingTest,
    ::testing::Values(
        INT_ALGORITHM(insertion_sort),
        INT_ALGORITHM(selection_sort),
        INT_ALGORITHM(exchange_sort),
        INT_ALGORITHM(bubble_sort),
        INT_ALGORITHM(optimized_bubble_sort),
        INT_ALGORITHM(binary_insertion_sort),
        INT_ALGORITHM(shaker_sort),
        INT_ALGORITHM(shell_sort),
        INT_ALGORITHM(quick_sort),
        INT_ALGORITHM(two_phase_merge_sort),
        INT_ALGORITHM(one_phase_merge_sort),
        INT_ALGORITHM(natural_merge_sort)),
    [](const ::testing::TestParamInfo<IntAlgorithm>& info) {
        return std::string(info.param.name);
    });

INSTANTIATE_TEST_SUITE_P(
    AllAlgorithms,
    DoubleSortingTest,
    ::testing::Values(
        DOUBLE_ALGORITHM(insertion_sort),
        DOUBLE_ALGORITHM(selection_sort),
        DOUBLE_ALGORITHM(exchange_sort),
        DOUBLE_ALGORITHM(bubble_sort),
        DOUBLE_ALGORITHM(optimized_bubble_sort),
        DOUBLE_ALGORITHM(binary_insertion_sort),
        DOUBLE_ALGORITHM(shaker_sort),
        DOUBLE_ALGORITHM(shell_sort),
        DOUBLE_ALGORITHM(quick_sort),
        DOUBLE_ALGORITHM(two_phase_merge_sort),
        DOUBLE_ALGORITHM(one_phase_merge_sort),
        DOUBLE_ALGORITHM(natural_merge_sort)),
    [](const ::testing::TestParamInfo<DoubleAlgorithm>& info) {
        return std::string(info.param.name);
    });

TEST_P(IntSortingTest, EmptyVector) {
    expect_int_result(GetParam().sort, {}, {});
}

TEST_P(IntSortingTest, SingleElement) {
    expect_int_result(GetParam().sort, {42}, {42});
}

TEST_P(IntSortingTest, AlreadySorted) {
    expect_int_result(GetParam().sort, {-9, -3, 0, 2, 8, 17}, {-9, -3, 0, 2, 8, 17});
}

TEST_P(IntSortingTest, ReverseSorted) {
    expect_int_result(GetParam().sort, {9, 7, 5, 3, 1, -1}, {-1, 1, 3, 5, 7, 9});
}

TEST_P(IntSortingTest, DuplicatesAndNegativeNumbers) {
    expect_int_result(GetParam().sort,
                      {4, -2, 4, 0, -2, 9, 1, 0, -7},
                      {-7, -2, -2, 0, 0, 1, 4, 4, 9});
}

TEST_P(IntSortingTest, MixedValues) {
    std::vector<int> input{12, 0, 5, 3, 11, 2, 8, 6, 1, 4, 7, 10, 9};
    auto expected = input;
    std::sort(expected.begin(), expected.end());
    expect_int_result(GetParam().sort, input, expected);
}

TEST_P(DoubleSortingTest, FractionalValues) {
    expect_double_result(GetParam().sort,
                         {2.5, -1.25, 0.0, 2.5, 3.75, -4.5},
                         {-4.5, -1.25, 0.0, 2.5, 2.5, 3.75});
}

TEST_P(DoubleSortingTest, AlreadySortedAndSingleElement) {
    expect_double_result(GetParam().sort, {-3.5, -1.0, 0.25, 4.75}, {-3.5, -1.0, 0.25, 4.75});
    expect_double_result(GetParam().sort, {0.125}, {0.125});
}

} // namespace
