#ifndef AST_HPP
#define AST_HPP

#include <memory>
#include <variant>

using Node = std::variant<struct And, struct Or, struct Not, struct Var>;

struct Var {
    char name;
};

struct And {
    std::unique_ptr<Node> lhs;
    std::unique_ptr<Node> rhs;
};

struct Or {
    std::unique_ptr<Node> lhs;
    std::unique_ptr<Node> rhs;
};

struct Not {
    std::unique_ptr<Node> expr;
};

#endif
