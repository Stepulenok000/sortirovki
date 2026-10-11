#pragma once
#include "external_sort_utils.h"
#include <cstddef>
#include <vector>

namespace sorting {

// One-phase simple merge: merged runs are distributed directly to the next pair of files.
template <class T>
void one_phase_merge_sort(std::vector<T>& values) {
    if (values.size() < 2) {
        return;
    }

    detail::TemporaryFiles temporary_files;
    const auto input_file = temporary_files.make_path("one_phase_input");
    auto first_file = temporary_files.make_path("one_phase_first");
    auto second_file = temporary_files.make_path("one_phase_second");
    auto output_first_file = temporary_files.make_path("one_phase_output_first");
    auto output_second_file = temporary_files.make_path("one_phase_output_second");
    detail::write_values(input_file, values);
    detail::distribute_fixed_runs<T>(input_file, first_file, second_file, 1);

    for (std::size_t run_length = 1; run_length < values.size();) {
        detail::merge_and_distribute_fixed_runs<T>(
            first_file, second_file, output_first_file, output_second_file, run_length);
        first_file.swap(output_first_file);
        second_file.swap(output_second_file);
        if (run_length > values.size() / 2) {
            break; // Avoid overflow in run_length *= 2.
        }
        run_length *= 2;
    }

    values = detail::read_values<T>(first_file);
}

extern template void one_phase_merge_sort<int>(std::vector<int>&);
extern template void one_phase_merge_sort<double>(std::vector<double>&);

} // namespace sorting
