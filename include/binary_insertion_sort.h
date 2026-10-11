#pragma once
#include <cstddef>
#include <vector>

namespace sorting {

template <class T>
void binary_insertion_sort(std::vector<T>& values) {
    for (std::size_t i = 1; i < values.size(); ++i) {
        T key = values[i];
        std::size_t left = 0;
        std::size_t right = i;
        // Find the upper-bound insertion point (stable for equivalent values).
        while (left < right) {
            const std::size_t middle = left + (right - left) / 2;
            if (key < values[middle]) {
                right = middle;
            } else {
                left = middle + 1;
            }
        }
        for (std::size_t j = i; j > left; --j) {
            values[j] = values[j - 1];
        }
        values[left] = key;
    }
}

extern template void binary_insertion_sort<int>(std::vector<int>&);
extern template void binary_insertion_sort<double>(std::vector<double>&);

} // namespace sorting
