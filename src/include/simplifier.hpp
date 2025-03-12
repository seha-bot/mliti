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

    void step() {
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

    void step_2() {
        for (std::size_t i = 0; i < grid.size(); i++) {
            for (std::size_t j = 0; j < grid.size(); j++) {
                if (i == j) {
                    continue;
                }
                auto xs = mismatches(i, j);
                int full_cnt = 0;
                int val_mismatch_cnt = 0;
                int dash_cnt = 0;
                std::size_t mismatch_i;
                for (auto [i, a, b] : xs) {
                    dash_cnt += a == 2;
                    if (a < 2 && b < 2 && a != b) {
                        mismatch_i = i;
                        ++val_mismatch_cnt;
                    }
                    ++full_cnt;
                }
                if (val_mismatch_cnt == 1 && dash_cnt == full_cnt - 1) {
                    grid[j][mismatch_i] = 2;
                }
            }
        }
    }

    void step_3() {
        for (std::size_t i = 0; i < grid.size(); i++) {
            for (std::size_t j = 0; j < grid.size(); j++) {
                if (i == j) {
                    continue;
                }
                for (std::size_t k = 0; k < grid.size(); k++) {
                    if (j == k) {
                        continue;
                    }
                    auto mism1 = mismatches(i, k);
                    auto mism2 = mismatches(k, j);
                    if (stdr::distance(mism1) == 2 && stdr::distance(mism2) == 2) {
                        auto v1 = stdr::find_if(mism1, [](auto&& x) { return get<1>(x) != 2; });
                        auto v2 = stdr::find_if(mism2, [](auto&& x) { return get<2>(x) != 2; });
                        assert(v1 != mism1.end() &&
                               v2 != mism2.end());  // can't think of a case, so this assert is here to discover it
                        if (v1 != mism1.end() && v2 != mism2.end() && get<1>(*v1) != get<2>(*v2)) {
                            grid.erase(grid.begin() + static_cast<std::ptrdiff_t>(k));
                            return;
                        }
                    }
                }
            }
        }
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

std::string simplify_table(Table const& table) {
    Simplifier simpl{table};

    for (int i = 0; i < 10; i++) {
        simpl.step();
        simpl.step_2();  // does this step require step 1 after?
        simpl.step_3();
    }
    return simpl.format();
}

#endif