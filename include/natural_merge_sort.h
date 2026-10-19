#pragma once
#include "merge_utils.h"
#include <filesystem>
#include <stdexcept>
#include <string>

namespace {
    template <typename T>
    void mergePhase(const std::string& b, const std::string& c, const std::string& dst) {
        merge_utils::Reader<T> rb(b), rc(c);
        merge_utils::Writer<T> out(dst);
        while (rb.valid() || rc.valid())
            merge_utils::mergeNatural(rb, rc, out);
    }
}

template <typename T>
void naturalMergeSort(const std::string& inputFile, const std::string& tempDir, bool keepTemp) {
    namespace fs = std::filesystem;
    if (!fs::exists(inputFile))
        throw std::runtime_error("Input file not found: " + inputFile);
    fs::create_directories(tempDir);

    const std::string fileB = merge_utils::tempPath(tempDir, "natural_b.txt");
    const std::string fileC = merge_utils::tempPath(tempDir, "natural_c.txt");

    while (true) {
        const std::size_t runs = merge_utils::distributeNatural<T>(inputFile, fileB, fileC);
        if (runs <= 1) break;
        mergePhase<T>(fileB, fileC, inputFile);
    }

    if (!keepTemp) {
        merge_utils::removeIfExists(fileB);
        merge_utils::removeIfExists(fileC);
    }
}

extern template void naturalMergeSort<int>(const std::string&, const std::string&, bool);
extern template void naturalMergeSort<double>(const std::string&, const std::string&, bool);