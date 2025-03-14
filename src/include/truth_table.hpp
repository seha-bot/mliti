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
        // TODO: please please make the ast descent in a decent way and not this hacky bullshit
        [](this auto&& self, std::set<char>& terms, Node const& tree) -> void {
            return std::visit(overloads{
                                  [&](And const& node) {
                                      self(terms, *node.lhs);
                                      self(terms, *node.rhs);
                                  },
                                  [&](Or const& node) {
                                      self(terms, *node.lhs);
                                      self(terms, *node.rhs);
                                  },
                                  [&](Not const& node) { self(terms, *node.expr); },
                                  [&](Var const& node) { terms.insert(node.name); },
                              },
                              tree);
        }(terms, *tree);

        // TODO: make a generator which returns an iterable of iterable bits
        for (unsigned i = 0; i != 1u << terms.size(); ++i) {
            auto vals = bits(i, std::ssize(terms));
            results.push_back([](this auto&& self, auto& vals, std::set<char>& terms, Node const& tree) -> bool {
                return std::visit(
                    overloads{
                        [&](And const& node) { return self(vals, terms, *node.lhs) && self(vals, terms, *node.rhs); },
                        [&](Or const& node) { return self(vals, terms, *node.lhs) || self(vals, terms, *node.rhs); },
                        [&](Not const& node) { return !self(vals, terms, *node.expr); },
                        [&](Var const& node) -> bool { return vals[distance(terms.begin(), terms.find(node.name))]; },
                    },
                    tree);
            }(vals, terms, *tree));
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
