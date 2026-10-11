#pragma once
#include <cstddef>
#include <utility>
#include <vector>

namespace sorting {

template <class T>
void selection_sort(std::vector<T>& values) {
    for (std::size_t i = 0; i < values.size(); ++i) {
        std::size_t min_index = i;
        for (std::size_t j = i + 1; j < values.size(); ++j) {
            if (values[j] < values[min_index]) {
                min_index = j;
            }
        }
        if (min_index != i) {
            std::swap(values[i], values[min_index]);
        }
    }
}

extern template void selection_sort<int>(std::vector<int>&);
extern template void selection_sort<double>(std::vector<double>&);

} // namespace sorting
