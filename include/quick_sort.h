#pragma once
#include <cstddef>
#include <utility>
#include <vector>

namespace sorting {
namespace detail {

template <class T>
void quick_sort_impl(std::vector<T>& values, std::ptrdiff_t low, std::ptrdiff_t high) {
    while (low < high) {
        const T pivot = values[low + (high - low) / 2];
        std::ptrdiff_t less = low;
        std::ptrdiff_t current = low;
        std::ptrdiff_t greater = high;

        // Three-way partition handles many equal values efficiently.
        while (current <= greater) {
            if (values[current] < pivot) {
                std::swap(values[less++], values[current++]);
            } else if (pivot < values[current]) {
                std::swap(values[current], values[greater--]);
            } else {
                ++current;
            }
        }

        // Recurse into the smaller side and iterate over the larger side.
        if (less - low < high - greater) {
            quick_sort_impl(values, low, less - 1);
            low = greater + 1;
        } else {
            quick_sort_impl(values, greater + 1, high);
            high = less - 1;
        }
    }
}

} // namespace detail

template <class T>
void quick_sort(std::vector<T>& values) {
    if (values.size() > 1) {
        detail::quick_sort_impl(values, 0,
            static_cast<std::ptrdiff_t>(values.size()) - 1);
    }
}

extern template void quick_sort<int>(std::vector<int>&);
extern template void quick_sort<double>(std::vector<double>&);

} // namespace sorting
