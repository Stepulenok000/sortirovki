#pragma once
#include "merge_utils.h"
#include <filesystem>
#include <stdexcept>
#include <string>
#include <cstddef>
#include <utility>

namespace {
    template <typename T>
    void mergePass(const std::string& src1, const std::string& src2,
        const std::string& dst1, const std::string& dst2, std::size_t len) {
        merge_utils::Reader<T> a(src1), b(src2);
        merge_utils::Writer<T> o1(dst1), o2(dst2);
        bool toFirst = true;
        while (a.valid() || b.valid()) {
            merge_utils::mergeFixed(a, b, toFirst ? o1 : o2, len);
            toFirst = !toFirst;
        }
    }
}

template <typename T>
void onePhaseMergeSort(const std::string& inputFile, const std::string& tempDir, std::size_t runSize, bool keepTemp) {
    namespace fs = std::filesystem;
    if (!fs::exists(inputFile))
        throw std::runtime_error("Input file not found: " + inputFile);
    if (runSize == 0) runSize = 1;
    fs::create_directories(tempDir);

    const std::string names[4] = {
        merge_utils::tempPath(tempDir, "one_phase_1.txt"),
        merge_utils::tempPath(tempDir, "one_phase_2.txt"),
        merge_utils::tempPath(tempDir, "one_phase_3.txt"),
        merge_utils::tempPath(tempDir, "one_phase_4.txt") };

    std::string src1 = names[0], src2 = names[1];
    std::string dst1 = names[2], dst2 = names[3];

    std::size_t runs = merge_utils::distributeSorted<T>(inputFile, src1, src2, runSize);

    if (runs > 0) {
        std::size_t len = runSize;
        while (runs > 1) {
            mergePass<T>(src1, src2, dst1, dst2, len);
            runs = (runs + 1) / 2;
            len *= 2;
            std::swap(src1, dst1);
            std::swap(src2, dst2);
        }
        merge_utils::replaceFile(src1, inputFile);
    }

    if (!keepTemp)
        for (const std::string& n : names) merge_utils::removeIfExists(n);
}

extern template void onePhaseMergeSort<int>(const std::string&, const std::string&, std::size_t, bool);
extern template void onePhaseMergeSort<double>(const std::string&, const std::string&, std::size_t, bool);