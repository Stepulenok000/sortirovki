#pragma once

#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace sorting::detail {

class TemporaryFiles {
public:
    TemporaryFiles() = default;
    TemporaryFiles(const TemporaryFiles&) = delete;
    TemporaryFiles& operator=(const TemporaryFiles&) = delete;

    ~TemporaryFiles() {
        for (const auto& path : paths_) {
            std::error_code error;
            std::filesystem::remove(path, error);
        }
    }

    std::filesystem::path make_path(const std::string& prefix) {
        static std::atomic<unsigned long long> counter{0};
        const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
        const auto id = counter.fetch_add(1, std::memory_order_relaxed);
        auto path = std::filesystem::temp_directory_path()
            / (prefix + "_" + std::to_string(stamp) + "_" + std::to_string(id) + ".txt");
        paths_.push_back(path);
        return path;
    }

private:
    std::vector<std::filesystem::path> paths_;
};

template <class T>
void write_values(const std::filesystem::path& path, const std::vector<T>& values) {
    std::ofstream output(path, std::ios::trunc);
    if (!output) {
        throw std::runtime_error("Cannot open temporary file for writing: " + path.string());
    }
    for (const T& value : values) {
        output << value << '\n';
    }
    if (!output) {
        throw std::runtime_error("Failed to write temporary file: " + path.string());
    }
}

template <class T>
std::vector<T> read_values(const std::filesystem::path& path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("Cannot open temporary file for reading: " + path.string());
    }
    std::vector<T> values;
    T value{};
    while (input >> value) {
        values.push_back(value);
    }
    if (!input.eof()) {
        throw std::runtime_error("Invalid data in temporary file: " + path.string());
    }
    return values;
}

template <class T>
std::vector<T> read_fixed_run(std::ifstream& input, std::size_t max_length) {
    std::vector<T> run;
    run.reserve(max_length);
    T value{};
    while (run.size() < max_length && input >> value) {
        run.push_back(value);
    }
    return run;
}

template <class T>
void write_run(std::ostream& output, const std::vector<T>& run) {
    for (const T& value : run) {
        output << value << '\n';
    }
}

template <class T>
std::vector<T> merge_runs(const std::vector<T>& left, const std::vector<T>& right) {
    std::vector<T> merged;
    merged.reserve(left.size() + right.size());
    std::size_t i = 0;
    std::size_t j = 0;
    while (i < left.size() && j < right.size()) {
        if (right[j] < left[i]) {
            merged.push_back(right[j++]);
        } else {
            merged.push_back(left[i++]);
        }
    }
    merged.insert(merged.end(), left.begin() + static_cast<std::ptrdiff_t>(i), left.end());
    merged.insert(merged.end(), right.begin() + static_cast<std::ptrdiff_t>(j), right.end());
    return merged;
}

// Split a file into fixed-length sorted runs, alternating runs between two files.
template <class T>
void distribute_fixed_runs(const std::filesystem::path& input_path,
                           const std::filesystem::path& first_path,
                           const std::filesystem::path& second_path,
                           std::size_t run_length) {
    std::ifstream input(input_path);
    std::ofstream first(first_path, std::ios::trunc);
    std::ofstream second(second_path, std::ios::trunc);
    if (!input || !first || !second) {
        throw std::runtime_error("Cannot open a temporary file during run distribution");
    }

    bool write_first = true;
    while (true) {
        auto run = read_fixed_run<T>(input, run_length);
        if (run.empty()) {
            break;
        }
        write_run(write_first ? static_cast<std::ostream&>(first)
                              : static_cast<std::ostream&>(second), run);
        write_first = !write_first;
    }
}

// Merge corresponding fixed-length runs and write all merged runs into one file.
template <class T>
void merge_fixed_runs(const std::filesystem::path& first_path,
                      const std::filesystem::path& second_path,
                      const std::filesystem::path& output_path,
                      std::size_t run_length) {
    std::ifstream first(first_path);
    std::ifstream second(second_path);
    std::ofstream output(output_path, std::ios::trunc);
    if (!first || !second || !output) {
        throw std::runtime_error("Cannot open a temporary file during run merging");
    }

    while (true) {
        auto left = read_fixed_run<T>(first, run_length);
        auto right = read_fixed_run<T>(second, run_length);
        if (left.empty() && right.empty()) {
            break;
        }
        write_run(output, merge_runs(left, right));
    }
}

// Merge corresponding fixed-length runs and distribute the results immediately.
template <class T>
void merge_and_distribute_fixed_runs(const std::filesystem::path& first_path,
                                     const std::filesystem::path& second_path,
                                     const std::filesystem::path& output_first_path,
                                     const std::filesystem::path& output_second_path,
                                     std::size_t run_length) {
    std::ifstream first(first_path);
    std::ifstream second(second_path);
    std::ofstream output_first(output_first_path, std::ios::trunc);
    std::ofstream output_second(output_second_path, std::ios::trunc);
    if (!first || !second || !output_first || !output_second) {
        throw std::runtime_error("Cannot open a temporary file during merge/distribution");
    }

    bool write_first = true;
    while (true) {
        auto left = read_fixed_run<T>(first, run_length);
        auto right = read_fixed_run<T>(second, run_length);
        if (left.empty() && right.empty()) {
            break;
        }
        const auto merged = merge_runs(left, right);
        write_run(write_first ? static_cast<std::ostream&>(output_first)
                              : static_cast<std::ostream&>(output_second), merged);
        write_first = !write_first;
    }
}

// Natural runs are detected from the values, even if the file contains blank separator lines.
template <class T>
std::size_t distribute_natural_runs(const std::filesystem::path& input_path,
                                    const std::filesystem::path& first_path,
                                    const std::filesystem::path& second_path) {
    std::ifstream input(input_path);
    std::ofstream first(first_path, std::ios::trunc);
    std::ofstream second(second_path, std::ios::trunc);
    if (!input || !first || !second) {
        throw std::runtime_error("Cannot open a temporary file during natural-run distribution");
    }

    std::size_t run_count = 0;
    bool write_first = true;
    std::vector<T> run;
    T value{};
    while (input >> value) {
        if (!run.empty() && value < run.back()) {
            auto& target = write_first ? first : second;
            write_run(target, run);
            target << '\n'; // Preserve run boundaries for the merge phase.
            ++run_count;
            write_first = !write_first;
            run.clear();
        }
        run.push_back(value);
    }
    if (!input.eof()) {
        throw std::runtime_error("Invalid data while reading natural-sort input");
    }
    if (!run.empty()) {
        auto& target = write_first ? first : second;
        write_run(target, run);
        target << '\n';
        ++run_count;
    }
    return run_count;
}

// Read one run using the blank-line separators written by distribute_natural_runs.
template <class T>
std::vector<T> read_delimited_run(std::ifstream& input) {
    std::vector<T> run;
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty() || line.find_first_not_of(" \t\r") == std::string::npos) {
            if (!run.empty()) {
                return run;
            }
            continue;
        }
        std::istringstream parser(line);
        T value{};
        if (!(parser >> value)) {
            throw std::runtime_error("Invalid value in natural-sort temporary file");
        }
        run.push_back(value);
    }
    return run;
}

template <class T>
void merge_natural_runs(const std::filesystem::path& first_path,
                        const std::filesystem::path& second_path,
                        const std::filesystem::path& output_path) {
    std::ifstream first(first_path);
    std::ifstream second(second_path);
    std::ofstream output(output_path, std::ios::trunc);
    if (!first || !second || !output) {
        throw std::runtime_error("Cannot open a temporary file during natural-run merging");
    }

    while (true) {
        auto left = read_delimited_run<T>(first);
        auto right = read_delimited_run<T>(second);
        if (left.empty() && right.empty()) {
            break;
        }
        write_run(output, merge_runs(left, right));
        output << '\n';
    }
}

} // namespace sorting::detail
