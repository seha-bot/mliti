#include <argparse/argparse.hpp>
#include <cstdlib>
#include <iostream>
#include <print>

#include "parsing.hpp"
#include "simplifier.hpp"

namespace fmt {

std::string format(Node const& tree) {
    return std::visit(
        overloads{
            [](And const& node) {
                auto is_short = [](Node const& node) {
                    return holds_alternative<Var>(node) || holds_alternative<And>(node) || holds_alternative<Not>(node);
                };
                bool const lhs_short = is_short(*node.lhs);
                bool const rhs_short = is_short(*node.rhs);
                std::string const lhs = format(*node.lhs);
                std::string const rhs = format(*node.rhs);
                if (lhs_short && rhs_short) {
                    return std::format("{} and {}", lhs, rhs);
                } else if (lhs_short) {
                    return std::format("{} and ({})", lhs, rhs);
                } else if (rhs_short) {
                    return std::format("({}) and {}", lhs, rhs);
                }
                return std::format("({}) and ({})", lhs, rhs);
            },
            [](Or const& node) {
                auto is_short = [](Node const& node) {
                    return holds_alternative<Var>(node) || holds_alternative<Or>(node) || holds_alternative<Not>(node);
                };
                bool const lhs_short = is_short(*node.lhs);
                bool const rhs_short = is_short(*node.rhs);
                std::string const lhs = format(*node.lhs);
                std::string const rhs = format(*node.rhs);
                if (lhs_short && rhs_short) {
                    return std::format("{} or {}", lhs, rhs);
                } else if (lhs_short) {
                    return std::format("{} or ({})", lhs, rhs);
                } else if (rhs_short) {
                    return std::format("({}) or {}", lhs, rhs);
                }
                return std::format("({}) or ({})", lhs, rhs);
            },
            [](Not const& node) {
                if (holds_alternative<Var>(*node.expr)) {
                    return std::format("n{}", format(*node.expr));
                }
                return std::format("n({})", format(*node.expr));
            },
            [](Var const& node) { return std::format("{}", node.name); },
        },
        tree);
}

std::string format_prefix(Node const& tree) {
    return std::visit(
        overloads{
            [](And const& node) { return std::format("a{}{}", format_prefix(*node.lhs), format_prefix(*node.rhs)); },
            [](Or const& node) { return std::format("o{}{}", format_prefix(*node.lhs), format_prefix(*node.rhs)); },
            [](Not const& node) { return std::format("n{}", format_prefix(*node.expr)); },
            [](Var const& node) { return std::format("{}", node.name); },
        },
        tree);
}

}  // namespace fmt

int main(int argc, char *argv[]) {
    argparse::ArgumentParser program("mliti");

    program.add_argument("expr").required();

    program.add_argument("-t", "--table")
        .default_value(false)
        .implicit_value(true)
        .help("prints the truth table for expr");

    program.add_argument("-s", "--show-expr")
        .default_value(false)
        .implicit_value(true)
        .help("prints the parsed expr for debugging purposes");

    program.add_argument("-p", "--prefix")
        .default_value(false)
        .implicit_value(true)
        .help("prints the parsed expr in prefix notation");

    try {
        program.parse_args(argc, argv);
    } catch (std::exception const& err) {
        std::cerr << err.what() << '\n' << program << '\n';
        return EXIT_FAILURE;
    }

    std::unique_ptr<Node> tree = parsing::parse(program.get<std::string>("expr"));
    if (!tree) {
        std::cerr << "parsing failed\n";
        return EXIT_FAILURE;
    }
    Table table{tree.get()};

    if (program.get<bool>("-t")) {
        table.print();
    }

    if (program.get<bool>("-s")) {
        std::println("{}", fmt::format(*tree));
    }

    if (program.get<bool>("-p")) {
        std::println("{}", fmt::format_prefix(*tree));
    }

    std::println("{}", simplify_table(table));
}

/*

axioms:

-------------------

1. idk how to put it into words cuz idk if it's more general than this

this doesn't work:
0-1
01-
-11

this works:
0-0
01-
-11

to

0-0
-11

-------------------

2. if there are two rows with 1 diff and one or more missing links

0--
101

to

0--
-01

-------------------

3. two rows with states which differ by only one value get those values removed

001
000

to

00-
00-

-------------------

4. for 2 identical rows, 1 can be removed

00-
00-

to

00-

-------------------

*/