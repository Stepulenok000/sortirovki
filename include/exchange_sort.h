#pragma once
#include <cstddef>
#include <utility>
#include <vector>

namespace sorting {

// Exchange sort: compares each element with every following element.
template <class T>
void exchange_sort(std::vector<T>& values) {
    for (std::size_t i = 0; i < values.size(); ++i) {
        for (std::size_t j = i + 1; j < values.size(); ++j) {
            if (values[j] < values[i]) {
                std::swap(values[i], values[j]);
            }
        }
    }
}

extern template void exchange_sort<int>(std::vector<int>&);
extern template void exchange_sort<double>(std::vector<double>&);

} // namespace sorting
