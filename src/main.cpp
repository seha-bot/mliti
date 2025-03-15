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

    Simplifier simpl{table};
    simpl.run();
    std::println("{}", simpl.format());
}
