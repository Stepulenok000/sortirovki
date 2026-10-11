#pragma once
#include <vector>

namespace sorting {

template <class T>
void insertion_sort(std::vector<T>& values) {
    for (std::size_t i = 1; i < values.size(); ++i) {
        T key = values[i];
        std::size_t j = i;
        while (j > 0 && key < values[j - 1]) {
            values[j] = values[j - 1];
            --j;
        }
        values[j] = key;
    }
}

extern template void insertion_sort<int>(std::vector<int>&);
extern template void insertion_sort<double>(std::vector<double>&);

} // namespace sorting
