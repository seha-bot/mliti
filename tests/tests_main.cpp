#include <catch2/catch_test_macros.hpp>
#include <stdexcept>
#include <string>
#include <string_view>

#include "parsing.hpp"
#include "simplifier.hpp"

std::string simplify(std::string_view expr) {
    auto tree = parsing::parse(expr);
    if (!tree) {
        throw std::runtime_error("parsing failed");
    }
    Table table{tree.get()};
    return simplify_table(table);
}

TEST_CASE("Simplifications", "[simplify]") {
    REQUIRE(simplify("nA or AnBC") == "nA or nBC");
    REQUIRE(simplify("nAD or nBD or ABC") == "nAD or nBD or ABC");
    REQUIRE(simplify("ABC or nAC or nBC") == "C");
    REQUIRE(simplify("nAB or nBC or nAC") == "nAB or nBC");
    REQUIRE(simplify("nAC or nAB or BC") == "nAC or nAB or BC");
    REQUIRE(simplify("n(AB(A or nBC) or n(n(AC) and n(BC)))") == "nAnB or nAnC or nBnC");
    REQUIRE(simplify("A or nA") == "t");
    REQUIRE(simplify("AnA") == "f");
    REQUIRE(simplify("A or B or C") == "A or B or C");
}
