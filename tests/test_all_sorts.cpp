#include <gtest/gtest.h>
#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

// Подключаем обычные сортировки
#include "binary_insertion_sort.h"
#include "bubble_sort.h"
#include "exchange_sort.h"
#include "insertion_sort.h"
#include "optimized_bubble_sort.h"
#include "quick_sort.h"
#include "selection_sort.h"
#include "shaker_sort.h"
#include "shell_sort.h"

// Подключаем внешние сортировки
#include "two_phase_merge_sort.h"
#include "one_phase_merge_sort.h"
#include "natural_merge_sort.h"

namespace {

    // ==========================================
    // 1. ТЕСТЫ ДЛЯ ОБЫЧНЫХ СОРТИРОВОК (через вектор)
    // ==========================================

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

    void expect_int_result(IntSortFunction sort, std::vector<int> actual, const std::vector<int>& expected) {
        sort(actual);
        EXPECT_EQ(actual, expected);
    }

    void expect_double_result(DoubleSortFunction sort, std::vector<double> actual, const std::vector<double>& expected) {
        sort(actual);
        EXPECT_EQ(actual, expected);
    }

#define INT_ALGORITHM(name) IntAlgorithm{#name, &sorting::name<int>}
#define DOUBLE_ALGORITHM(name) DoubleAlgorithm{#name, &sorting::name<double>}

    INSTANTIATE_TEST_SUITE_P(
        AllAlgorithms, IntSortingTest,
        ::testing::Values(
            INT_ALGORITHM(insertion_sort), INT_ALGORITHM(selection_sort),
            INT_ALGORITHM(exchange_sort), INT_ALGORITHM(bubble_sort),
            INT_ALGORITHM(optimized_bubble_sort), INT_ALGORITHM(binary_insertion_sort),
            INT_ALGORITHM(shaker_sort), INT_ALGORITHM(shell_sort),
            INT_ALGORITHM(quick_sort)
        ),
        [](const ::testing::TestParamInfo<IntAlgorithm>& info) { return std::string(info.param.name); }
    );

    INSTANTIATE_TEST_SUITE_P(
        AllAlgorithms, DoubleSortingTest,
        ::testing::Values(
            DOUBLE_ALGORITHM(insertion_sort), DOUBLE_ALGORITHM(selection_sort),
            DOUBLE_ALGORITHM(exchange_sort), DOUBLE_ALGORITHM(bubble_sort),
            DOUBLE_ALGORITHM(optimized_bubble_sort), DOUBLE_ALGORITHM(binary_insertion_sort),
            DOUBLE_ALGORITHM(shaker_sort), DOUBLE_ALGORITHM(shell_sort),
            DOUBLE_ALGORITHM(quick_sort)
        ),
        [](const ::testing::TestParamInfo<DoubleAlgorithm>& info) { return std::string(info.param.name); }
    );

    TEST_P(IntSortingTest, EmptyVector) { expect_int_result(GetParam().sort, {}, {}); }
    TEST_P(IntSortingTest, SingleElement) { expect_int_result(GetParam().sort, { 42 }, { 42 }); }
    TEST_P(IntSortingTest, AlreadySorted) { expect_int_result(GetParam().sort, { -9, -3, 0, 2, 8, 17 }, { -9, -3, 0, 2, 8, 17 }); }
    TEST_P(IntSortingTest, ReverseSorted) { expect_int_result(GetParam().sort, { 9, 7, 5, 3, 1, -1 }, { -1, 1, 3, 5, 7, 9 }); }
    TEST_P(IntSortingTest, DuplicatesAndNegativeNumbers) {
        expect_int_result(GetParam().sort, { 4, -2, 4, 0, -2, 9, 1, 0, -7 }, { -7, -2, -2, 0, 0, 1, 4, 4, 9 });
    }
    TEST_P(IntSortingTest, MixedValues) {
        std::vector<int> input{ 12, 0, 5, 3, 11, 2, 8, 6, 1, 4, 7, 10, 9 };
        auto expected = input;
        std::sort(expected.begin(), expected.end());
        expect_int_result(GetParam().sort, input, expected);
    }

    TEST_P(DoubleSortingTest, FractionalValues) {
        expect_double_result(GetParam().sort, { 2.5, -1.25, 0.0, 2.5, 3.75, -4.5 }, { -4.5, -1.25, 0.0, 2.5, 2.5, 3.75 });
    }

    // ==========================================
    // 2. ТЕСТЫ ДЛЯ ВНЕШНИХ СОРТИРОВОК (через файлы)
    // ==========================================

    // Вспомогательная функция для записи вектора в файл
    template <typename T>
    void writeToFile(const std::string& filename, const std::vector<T>& data) {
        std::ofstream out(filename);
        for (const auto& val : data) out << val << "\n";
    }

    // Вспомогательная функция для чтения вектора из файла
    template <typename T>
    std::vector<T> readFromFile(const std::string& filename) {
        std::ifstream in(filename);
        std::vector<T> data;
        T val;
        while (in >> val) data.push_back(val);
        return data;
    }

    class ExternalSortTest : public ::testing::Test {
    protected:
        std::string testFile = "test_input.txt";
        std::string tempDir = "test_temp";

        void SetUp() override {
            std::filesystem::create_directories(tempDir);
        }

        void TearDown() override {
            std::filesystem::remove(testFile);
            std::filesystem::remove_all(tempDir);
        }
    };

    TEST_F(ExternalSortTest, TwoPhaseMergeSortTest) {
        std::vector<int> input{ 5, 1, 4, 2, 8 };
        std::vector<int> expected{ 1, 2, 4, 5, 8 };
        writeToFile(testFile, input);

        twoPhaseMergeSort<int>(testFile, tempDir, 2, false);

        std::vector<int> result = readFromFile<int>(testFile);
        EXPECT_EQ(result, expected);
    }

    TEST_F(ExternalSortTest, OnePhaseMergeSortTest) {
        std::vector<int> input{ 5, 1, 4, 2, 8 };
        std::vector<int> expected{ 1, 2, 4, 5, 8 };
        writeToFile(testFile, input);

        onePhaseMergeSort<int>(testFile, tempDir, 2, false);

        std::vector<int> result = readFromFile<int>(testFile);
        EXPECT_EQ(result, expected);
    }

    TEST_F(ExternalSortTest, NaturalMergeSortTest) {
        std::vector<int> input{ 5, 1, 4, 2, 8 };
        std::vector<int> expected{ 1, 2, 4, 5, 8 };
        writeToFile(testFile, input);

        naturalMergeSort<int>(testFile, tempDir, false);

        std::vector<int> result = readFromFile<int>(testFile);
        EXPECT_EQ(result, expected);
    }

} // namespace