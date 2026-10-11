#pragma once
#include <cstddef>
#include <utility>
#include <vector>

namespace sorting {

template <class T>
void shaker_sort(std::vector<T>& values) {
    if (values.size() < 2) {
        return;
    }

    std::size_t left = 0;
    std::size_t right = values.size() - 1;
    while (left < right) {
        for (std::size_t i = left; i < right; ++i) {
            if (values[i + 1] < values[i]) {
                std::swap(values[i], values[i + 1]);
            }
        }
        --right;

        for (std::size_t i = right; i > left; --i) {
            if (values[i] < values[i - 1]) {
                std::swap(values[i], values[i - 1]);
            }
        }
        ++left;
    }
}

extern template void shaker_sort<int>(std::vector<int>&);
extern template void shaker_sort<double>(std::vector<double>&);

} // namespace sorting
