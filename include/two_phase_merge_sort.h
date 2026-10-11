#pragma once
#include "external_sort_utils.h"
#include <cstddef>
#include <vector>

namespace sorting {

// Two-phase simple merge: distribute fixed-length runs, then merge them back.
template <class T>
void two_phase_merge_sort(std::vector<T>& values) {
    if (values.size() < 2) {
        return;
    }

    detail::TemporaryFiles temporary_files;
    const auto main_file = temporary_files.make_path("two_phase_main");
    const auto first_file = temporary_files.make_path("two_phase_first");
    const auto second_file = temporary_files.make_path("two_phase_second");
    detail::write_values(main_file, values);

    for (std::size_t run_length = 1; run_length < values.size();) {
        detail::distribute_fixed_runs<T>(main_file, first_file, second_file, run_length);
        detail::merge_fixed_runs<T>(first_file, second_file, main_file, run_length);
        if (run_length > values.size() / 2) {
            break; // Avoid overflow in run_length *= 2.
        }
        run_length *= 2;
    }
    values = detail::read_values<T>(main_file);
}

extern template void two_phase_merge_sort<int>(std::vector<int>&);
extern template void two_phase_merge_sort<double>(std::vector<double>&);

} // namespace sorting
