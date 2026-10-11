#pragma once
#include "external_sort_utils.h"
#include <vector>

namespace sorting {

// Natural merge: identify maximal non-decreasing runs before every merge pass.
template <class T>
void natural_merge_sort(std::vector<T>& values) {
    if (values.size() < 2) {
        return;
    }

    detail::TemporaryFiles temporary_files;
    const auto main_file = temporary_files.make_path("natural_main");
    const auto first_file = temporary_files.make_path("natural_first");
    const auto second_file = temporary_files.make_path("natural_second");
    detail::write_values(main_file, values);

    while (true) {
        const std::size_t run_count =
            detail::distribute_natural_runs<T>(main_file, first_file, second_file);
        if (run_count <= 1) {
            values = detail::read_values<T>(first_file);
            return;
        }
        detail::merge_natural_runs<T>(first_file, second_file, main_file);
    }
}

extern template void natural_merge_sort<int>(std::vector<int>&);
extern template void natural_merge_sort<double>(std::vector<double>&);

} // namespace sorting
