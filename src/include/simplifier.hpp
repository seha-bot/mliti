#ifndef SIMPLIFIER_HPP
#define SIMPLIFIER_HPP

#include <cassert>

#include "truth_table.hpp"

struct Simplifier {
    Table const& m_table;
    std::vector<std::vector<int>> grid;

    Simplifier(Table const& table) : m_table(table) {
        for (auto [i, result] : table.results | stdv::enumerate) {
            if (result) {
                grid.push_back(bits(static_cast<unsigned>(i), std::ssize(table.terms)) | stdr::to<std::vector<int>>());
            }
        }
    }

    auto mismatches(std::size_t i, std::size_t j) {
        return stdv::zip(stdv::iota(0uz), grid[i], grid[j]) |
               stdv::filter([](auto&& x) { return get<1>(x) != get<2>(x); });
    }

    // Simplifies: (A or nA), (A or A)
    void step_1() {
        for (;;) {
            bool made_change = false;
            for (std::size_t i = 0; i < grid.size(); i++) {
                for (std::size_t j = i + 1; j < grid.size(); j++) {
                    auto mism = mismatches(i, j);
                    auto mism_cnt = stdr::distance(mism);
                    if (mism_cnt <= 1) {
                        if (mism_cnt == 1) {
                            grid[i][get<0>(mism.front())] = 2;
                        }
                        grid.erase(grid.begin() + static_cast<std::ptrdiff_t>(j--));
                        made_change = true;
                    }
                }
            }

            if (!made_change) {
                break;
            }
        }
    }

    // Simplifies: (A or nAB)
    void step_2() {
        for (std::size_t i = 0; i < grid.size(); i++) {
            for (std::size_t j = i + 1; j < grid.size(); j++) {
                auto xs = mismatches(i, j);
                int full_cnt = 0;
                int val_mismatch_cnt = 0;
                int dash_cnt_a = 0;
                int dash_cnt_b = 0;
                std::size_t mismatch_i;
                for (auto [i, a, b] : xs) {
                    dash_cnt_a += a == 2;
                    dash_cnt_b += b == 2;
                    if (a != 2 && b != 2 && a != b) {
                        mismatch_i = i;
                        ++val_mismatch_cnt;
                    }
                    ++full_cnt;
                }
                if (val_mismatch_cnt == 1) {
                    if (dash_cnt_a == full_cnt - 1) {
                        grid[j][mismatch_i] = 2;
                    } else if (dash_cnt_b == full_cnt - 1) {
                        grid[i][mismatch_i] = 2;
                    }
                }
            }
        }
    }

    // Simplifies: redundant rows
    void step_3() {}

    void run() {
        step_1();
        step_2();  // does this step require step 1 again after?
        step_3();
    }

    void print() {
        std::println("Rows:");
        for (auto& row : grid) {
            for (int x : row) {
                std::print("{}", x == 2 ? '-' : char(x + '0'));
            }
            std::println();
        }
    }

    std::string format() {
        if (grid.empty()) {
            return "f";
        }

        std::string str;
        for (std::size_t i = 0; i < grid[0].size(); i++) {
            if (grid[0][i] == 1) {
                str.push_back(char(i + 'A'));
            } else if (grid[0][i] == 0) {
                str.push_back('n');
                str.push_back(char(i + 'A'));
            }
        }
        for (auto& row : grid | stdv::drop(1)) {
            str.append(" or ");
            for (std::size_t i = 0; i < row.size(); i++) {
                if (row[i] == 1) {
                    str.push_back(char(i + 'A'));
                } else if (row[i] == 0) {
                    str.push_back('n');
                    str.push_back(char(i + 'A'));
                }
            }
        }

        if (str.empty()) {
            return "t";
        }
        return str;
    }
};

#endif
