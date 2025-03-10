#ifndef TRUTH_TABLE_HPP
#define TRUTH_TABLE_HPP

#include <iterator>
#include <print>
#include <set>
#include <vector>

#include "ast.hpp"
#include "utils.hpp"

struct Table {
    std::set<char> terms;
    std::vector<bool> results;

    Table(Node *tree) {
        [this](this auto&& self, Node const& tree) -> void {
            return std::visit(overloads{
                                  [&](And const& node) {
                                      self(*node.lhs);
                                      self(*node.rhs);
                                  },
                                  [&](Or const& node) {
                                      self(*node.lhs);
                                      self(*node.rhs);
                                  },
                                  [&](Not const& node) { self(*node.expr); },
                                  [&](Var const& node) { terms.insert(node.name); },
                              },
                              tree);
        }(*tree);

        for (unsigned i = 0; i != 1 << terms.size(); ++i) {
            auto vals = bits(i, std::ssize(terms));
            results.push_back([this, &vals](this auto&& self, Node const& tree) -> bool {
                return std::visit(
                    overloads{
                        [&](And const& node) { return self(*node.lhs) && self(*node.rhs); },
                        [&](Or const& node) { return self(*node.lhs) || self(*node.rhs); },
                        [&](Not const& node) { return !self(*node.expr); },
                        [&](Var const& node) -> bool { return vals[distance(terms.begin(), terms.find(node.name))]; },
                    },
                    tree);
            }(*tree));
        }
    }

    void print() {
        for (char term : terms) {
            std::print("{} ", term);
        }
        std::println("| R");
        for (unsigned i = 0; i < results.size(); ++i) {
            for (int bit : bits(i, std::ssize(terms))) {
                std::print("{} ", bit);
            }
            std::println("| {}", results[i] ? 1 : 0);
        }
    }

    void print_results() {
        for (int result : results) {
            std::print("{}", result);
        }
        std::println();
    }

    void ones() {
        for (char term : terms) {
            std::print("{} ", term);
        }
        std::println("| R");
        for (unsigned i = 0; i < results.size(); ++i) {
            for (int bit : bits(i, std::ssize(terms))) {
                std::print("{} ", bit);
            }
            std::println("| {}", results[i] ? 1 : 0);
        }
    }
};

#endif