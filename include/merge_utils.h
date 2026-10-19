#pragma once
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <cstddef>
#include <cstdio>

namespace merge_utils {

    inline std::string tempPath(const std::string& dir, const std::string& name) {
        return (std::filesystem::path(dir) / name).string();
    }

    inline void removeIfExists(const std::string& path) {
        std::error_code ec;
        std::filesystem::remove(path, ec);
    }

    inline void replaceFile(const std::string& src, const std::string& dst) {
        std::error_code ec;
        std::filesystem::rename(src, dst, ec);
        if (ec) {
            std::filesystem::copy_file(src, dst, std::filesystem::copy_options::overwrite_existing, ec);
            std::filesystem::remove(src, ec);
        }
    }

    template <typename T>
    class Reader {
        std::ifstream in;
        T current;
        bool has_current = false;
    public:
        explicit Reader(const std::string& path) : in(path) { advance(); }

        bool valid() const { return has_current; }
        T peek() const { return current; }

        void advance() {
            if (in >> current) {
                has_current = true;
            }
            else {
                has_current = false;
            }
        }
    };

    template <typename T>
    class Writer {
        std::ofstream out;
    public:
        explicit Writer(const std::string& path) : out(path) {}
        void write(const T& value) { out << value << "\n"; }
    };

    template <typename T>
    void mergeFixed(Reader<T>& a, Reader<T>& b, Writer<T>& out, std::size_t len) {
        std::size_t countA = 0, countB = 0;
        while ((countA < len && a.valid()) || (countB < len && b.valid())) {
            if (countA < len && a.valid() && (countB >= len || !b.valid() || a.peek() <= b.peek())) {
                out.write(a.peek());
                a.advance();
                countA++;
            }
            else if (countB < len && b.valid()) {
                out.write(b.peek());
                b.advance();
                countB++;
            }
        }
    }

    template <typename T>
    void mergeNatural(Reader<T>& a, Reader<T>& b, Writer<T>& out) {
        while (a.valid() || b.valid()) {
            if (a.valid() && (!b.valid() || a.peek() <= b.peek())) {
                out.write(a.peek());
                a.advance();
            }
            else if (b.valid()) {
                out.write(b.peek());
                b.advance();
            }
        }
    }

    template <typename T>
    std::size_t distributeSorted(const std::string& input, const std::string& out1, const std::string& out2, std::size_t runSize) {
        std::ifstream in(input);
        std::ofstream o1(out1), o2(out2);
        std::vector<T> buf(runSize);
        std::size_t runs = 0;
        bool toFirst = true;
        while (in) {
            buf.clear();
            T val;
            while (buf.size() < runSize && (in >> val)) buf.push_back(val);
            if (buf.empty()) break;
            std::sort(buf.begin(), buf.end());
            std::ofstream& out = toFirst ? o1 : o2;
            for (const auto& x : buf) out << x << "\n";
            toFirst = !toFirst;
            runs++;
        }
        return runs;
    }

    template <typename T>
    void distributeFixed(const std::string& input, const std::string& out1, const std::string& out2, std::size_t runSize) {
        distributeSorted<T>(input, out1, out2, runSize); // ƒл€ фиксированного размера куски уже отсортированы
    }

    template <typename T>
    std::size_t distributeNatural(const std::string& input, const std::string& out1, const std::string& out2) {
        std::ifstream in(input);
        std::ofstream o1(out1), o2(out2);
        T val, prev;
        std::size_t runs = 0;
        bool toFirst = true;
        bool first = true;
        bool run_started = false;

        while (in >> val) {
            if (first) {
                prev = val;
                first = false;
                run_started = true;
            }
            else {
                if (val < prev) { //  онец серии
                    (toFirst ? o1 : o2) << "\n"; // разделитель серий
                    toFirst = !toFirst;
                    runs++;
                }
            }
            (toFirst ? o1 : o2) << val << "\n";
            prev = val;
        }
        if (run_started) {
            (toFirst ? o1 : o2) << "\n";
            runs++;
        }
        return runs;
    }
}