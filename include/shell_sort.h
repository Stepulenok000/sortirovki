#pragma once
#include <cstddef>
#include <vector>

namespace sorting {

template <class T>
void shell_sort(std::vector<T>& values) {
    for (std::size_t gap = values.size() / 2; gap > 0; gap /= 2) {
        for (std::size_t i = gap; i < values.size(); ++i) {
            T key = values[i];
            std::size_t j = i;
            while (j >= gap && key < values[j - gap]) {
                values[j] = values[j - gap];
                j -= gap;
            }
            values[j] = key;
        }
    }
}

extern template void shell_sort<int>(std::vector<int>&);
extern template void shell_sort<double>(std::vector<double>&);

} // namespace sorting
