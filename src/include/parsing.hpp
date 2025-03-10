#ifndef PARSING_HPP
#define PARSING_HPP

#include <string_view>
#include <cctype>

#include "ast.hpp"

namespace parsing {

bool parse_str(std::string_view& str, std::string_view val) {
    if (str.starts_with(val)) {
        str = str.substr(val.size());
        return true;
    }
    return false;
}

std::unique_ptr<Node> variable(std::string_view& str) {
    if (!str.empty() && std::isupper(str[0])) {
        char c = str[0];
        str = str.substr(1);
        return std::make_unique<Node>(Var{c});
    }
    return nullptr;
}

std::unique_ptr<Node> expr(std::string_view& str);

std::unique_ptr<Node> factor(std::string_view& str) {
    bool is_not = parse_str(str, "n");

    if (auto x = variable(str)) {
        return is_not ? std::make_unique<Node>(Not{std::move(x)}) : std::move(x);
    }

    if (parse_str(str, "(")) {
        if (auto x = expr(str)) {
            if (parse_str(str, ")")) {
                return is_not ? std::make_unique<Node>(Not{std::move(x)}) : std::move(x);
            }
        }
    }

    return nullptr;
}

std::unique_ptr<Node> term(std::string_view& str) {
    auto x = factor(str);
    if (!x) {
        return nullptr;
    }

    for (;;) {
        parse_str(str, " and ");
        auto f = factor(str);
        if (!f) {
            return x;
        }
        x = std::make_unique<Node>(And{std::move(x), std::move(f)});
    }
}

std::unique_ptr<Node> expr(std::string_view& str) {
    auto x = term(str);
    if (!x) {
        return nullptr;
    }

    for (;;) {
        if (!parse_str(str, " or ")) {
            return x;
        }
        auto t = term(str);
        if (!t) {
            return x;
        }
        x = std::make_unique<Node>(Or{std::move(x), std::move(t)});
    }
}

std::unique_ptr<Node> parse(std::string_view str) {
    // Expr <- Term (' or ' Term)*
    // Term <- Factor (' and ' Factor / Factor)*
    // Factor <- 'n'? (Variable / '(' Expr ')')
    // Variable <- [A-Z]
    return expr(str);
}

}  // namespace parsing

#endif