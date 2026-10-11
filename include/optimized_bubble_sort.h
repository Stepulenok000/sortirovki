#pragma once
#include <cstddef>
#include <utility>
#include <vector>

namespace sorting {

template <class T>
void optimized_bubble_sort(std::vector<T>& values) {
    std::size_t end = values.size();
    while (end > 1) {
        std::size_t last_swap = 0;
        for (std::size_t i = 1; i < end; ++i) {
            if (values[i] < values[i - 1]) {
                std::swap(values[i], values[i - 1]);
                last_swap = i;
            }
        }
        // No swap means the sequence is already sorted.
        if (last_swap == 0) {
            break;
        }
        // Everything at and after the last swapped index is in its final place.
        end = last_swap;
    }
}

extern template void optimized_bubble_sort<int>(std::vector<int>&);
extern template void optimized_bubble_sort<double>(std::vector<double>&);

} // namespace sorting
