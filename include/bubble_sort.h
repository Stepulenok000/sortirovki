#pragma once
#include <cstddef>
#include <utility>
#include <vector>

namespace sorting {

template <class T>
void bubble_sort(std::vector<T>& values) {
    for (std::size_t end = values.size(); end > 1; --end) {
        for (std::size_t i = 1; i < end; ++i) {
            if (values[i] < values[i - 1]) {
                std::swap(values[i], values[i - 1]);
            }
        }
    }
}

extern template void bubble_sort<int>(std::vector<int>&);
extern template void bubble_sort<double>(std::vector<double>&);

} // namespace sorting
