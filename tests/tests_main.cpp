#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "parsing.hpp"
#include "simplifier.hpp"
#include "utils.hpp"

// std::string simplify(std::string_view expr) {
//     auto tree = parsing::parse(expr);
//     if (!tree) {
//         throw std::runtime_error("parsing failed");
//     }
//     Table table{tree.get()};
//     Simplifier simpl{table};
//     simpl.run();
//     return simpl.format();
// }

static bool no_corruption_for(int var_cnt) {
    Table table;
    for (int i = 0; i < var_cnt; ++i) {
        table.terms.insert('A' + static_cast<char>(i));
    }

    // Try to simplify each possible result.
    for (unsigned i = 0; i != 1u << var_cnt; ++i) {
        table.results = bits(i, 1u << var_cnt) | stdr::to<std::vector<bool>>();

        Simplifier simpl{table};
        simpl.run();

        // TODO: this process can be so much simplified...
        auto expr = simpl.format();
        if (expr == "f") {
            // all results must be false in this case
            if (i != 0) {
                throw std::runtime_error("didn't get false for " + table.fmt_results());
            }
            continue;
        }
        if (expr == "t") {
            // all results must be true in this case
            if (i != (1u << var_cnt) - 1) {
                throw std::runtime_error("didn't get true for " + table.fmt_results());
            }
            continue;
        }

        auto tree = parsing::parse(expr);
        if (!tree) {
            throw std::runtime_error("parsing failed for \"" + expr + '"');
        }
        Table simpl_table;
        simpl_table.terms = table.terms;
        simpl_table.retrace(tree.get());
        if (table.results != simpl_table.results) {
            throw std::runtime_error("result mismatch: " + table.fmt_results() + " " + simpl_table.fmt_results());
        }
    }

    return true;
}

static std::string best_repr(int var_cnt, unsigned res) {
    Table table;
    for (int i = 0; i < var_cnt; ++i) {
        table.terms.insert('A' + static_cast<char>(i));
    }
    table.results = bits(res, 1u << var_cnt) | stdr::to<std::vector<bool>>();
    Simplifier simpl{table};
    simpl.run();
    return simpl.format();
}

TEST_CASE("Corruption", "[corruption]") {
    REQUIRE(no_corruption_for(1));
    REQUIRE(no_corruption_for(2));
    REQUIRE(no_corruption_for(3));
    REQUIRE(no_corruption_for(4));
}

TEST_CASE("Degree 1", "[deg1]") {
    REQUIRE(best_repr(1, 0b00) == "f");
    REQUIRE(best_repr(1, 0b01) == "A");
    REQUIRE(best_repr(1, 0b10) == "nA");
    REQUIRE(best_repr(1, 0b11) == "t");
}

TEST_CASE("Degree 2", "[deg2]") {
    REQUIRE(best_repr(2, 0b0000) == "f");
    REQUIRE(best_repr(2, 0b0001) == "AB");
    REQUIRE(best_repr(2, 0b0010) == "AnB");
    REQUIRE(best_repr(2, 0b0011) == "A");
    REQUIRE(best_repr(2, 0b0100) == "nAB");
    REQUIRE(best_repr(2, 0b0101) == "B");
    REQUIRE(best_repr(2, 0b0110) == "nAB or AnB");
    REQUIRE(best_repr(2, 0b0111) == "B or A");
    REQUIRE(best_repr(2, 0b1000) == "nAnB");
    REQUIRE(best_repr(2, 0b1001) == "nAnB or AB");
    REQUIRE(best_repr(2, 0b1010) == "nB");
    REQUIRE(best_repr(2, 0b1011) == "nB or A");
    REQUIRE(best_repr(2, 0b1100) == "nA");
    REQUIRE(best_repr(2, 0b1101) == "nA or B");
    REQUIRE(best_repr(2, 0b1110) == "nA or nB");
    REQUIRE(best_repr(2, 0b1111) == "t");
}

TEST_CASE("Degree 3", "[deg3]") {
    REQUIRE(best_repr(3, 0b00000000) == "f");
    REQUIRE(best_repr(3, 0b00000001) == "ABC");
    REQUIRE(best_repr(3, 0b00000010) == "ABnC");
    REQUIRE(best_repr(3, 0b00000011) == "AB");
    REQUIRE(best_repr(3, 0b00000100) == "AnBC");
    REQUIRE(best_repr(3, 0b00000101) == "AC");
    REQUIRE(best_repr(3, 0b00000110) == "AnBC or ABnC");
    REQUIRE(best_repr(3, 0b00000111) == "AC or AB");
    REQUIRE(best_repr(3, 0b00001000) == "AnBnC");
    REQUIRE(best_repr(3, 0b00001001) == "AnBnC or ABC");
    REQUIRE(best_repr(3, 0b00001010) == "AnC");
    REQUIRE(best_repr(3, 0b00001011) == "AnC or AB");
    REQUIRE(best_repr(3, 0b00001100) == "AnB");
    REQUIRE(best_repr(3, 0b00001101) == "AnB or AC");
    REQUIRE(best_repr(3, 0b00001110) == "AnB or AnC");
    REQUIRE(best_repr(3, 0b00001111) == "A");
    REQUIRE(best_repr(3, 0b00010000) == "nABC");
    REQUIRE(best_repr(3, 0b00010001) == "BC");
    REQUIRE(best_repr(3, 0b00010010) == "");
    REQUIRE(best_repr(3, 0b00010011) == "");
    REQUIRE(best_repr(3, 0b00010100) == "");
    REQUIRE(best_repr(3, 0b00010101) == "");
    REQUIRE(best_repr(3, 0b00010110) == "");
    REQUIRE(best_repr(3, 0b00010111) == "");
    REQUIRE(best_repr(3, 0b00011000) == "");
    REQUIRE(best_repr(3, 0b00011001) == "");
    REQUIRE(best_repr(3, 0b00011010) == "");
    REQUIRE(best_repr(3, 0b00011011) == "");
    REQUIRE(best_repr(3, 0b00011100) == "");
    REQUIRE(best_repr(3, 0b00011101) == "");
    REQUIRE(best_repr(3, 0b00011110) == "");
    REQUIRE(best_repr(3, 0b00011111) == "");
    REQUIRE(best_repr(3, 0b00100000) == "");
    REQUIRE(best_repr(3, 0b00100001) == "");
    REQUIRE(best_repr(3, 0b00100010) == "");
    REQUIRE(best_repr(3, 0b00100011) == "");
    REQUIRE(best_repr(3, 0b00100100) == "");
    REQUIRE(best_repr(3, 0b00100101) == "");
    REQUIRE(best_repr(3, 0b00100110) == "");
    REQUIRE(best_repr(3, 0b00100111) == "");
    REQUIRE(best_repr(3, 0b00101000) == "");
    REQUIRE(best_repr(3, 0b00101001) == "");
    REQUIRE(best_repr(3, 0b00101010) == "");
    REQUIRE(best_repr(3, 0b00101011) == "");
    REQUIRE(best_repr(3, 0b00101100) == "");
    REQUIRE(best_repr(3, 0b00101101) == "");
    REQUIRE(best_repr(3, 0b00101110) == "");
    REQUIRE(best_repr(3, 0b00101111) == "");
    REQUIRE(best_repr(3, 0b00110000) == "");
    REQUIRE(best_repr(3, 0b00110001) == "");
    REQUIRE(best_repr(3, 0b00110010) == "");
    REQUIRE(best_repr(3, 0b00110011) == "");
    REQUIRE(best_repr(3, 0b00110100) == "");
    REQUIRE(best_repr(3, 0b00110101) == "");
    REQUIRE(best_repr(3, 0b00110110) == "");
    REQUIRE(best_repr(3, 0b00110111) == "");
    REQUIRE(best_repr(3, 0b00111000) == "");
    REQUIRE(best_repr(3, 0b00111001) == "");
    REQUIRE(best_repr(3, 0b00111010) == "");
    REQUIRE(best_repr(3, 0b00111011) == "");
    REQUIRE(best_repr(3, 0b00111100) == "");
    REQUIRE(best_repr(3, 0b00111101) == "");
    REQUIRE(best_repr(3, 0b00111110) == "");
    REQUIRE(best_repr(3, 0b00111111) == "");
    REQUIRE(best_repr(3, 0b01000000) == "");
    REQUIRE(best_repr(3, 0b01000001) == "");
    REQUIRE(best_repr(3, 0b01000010) == "");
    REQUIRE(best_repr(3, 0b01000011) == "");
    REQUIRE(best_repr(3, 0b01000100) == "");
    REQUIRE(best_repr(3, 0b01000101) == "");
    REQUIRE(best_repr(3, 0b01000110) == "");
    REQUIRE(best_repr(3, 0b01000111) == "");
    REQUIRE(best_repr(3, 0b01001000) == "");
    REQUIRE(best_repr(3, 0b01001001) == "");
    REQUIRE(best_repr(3, 0b01001010) == "");
    REQUIRE(best_repr(3, 0b01001011) == "");
    REQUIRE(best_repr(3, 0b01001100) == "");
    REQUIRE(best_repr(3, 0b01001101) == "");
    REQUIRE(best_repr(3, 0b01001110) == "");
    REQUIRE(best_repr(3, 0b01001111) == "");
    REQUIRE(best_repr(3, 0b01010000) == "");
    REQUIRE(best_repr(3, 0b01010001) == "");
    REQUIRE(best_repr(3, 0b01010010) == "");
    REQUIRE(best_repr(3, 0b01010011) == "");
    REQUIRE(best_repr(3, 0b01010100) == "");
    REQUIRE(best_repr(3, 0b01010101) == "");
    REQUIRE(best_repr(3, 0b01010110) == "");
    REQUIRE(best_repr(3, 0b01010111) == "");
    REQUIRE(best_repr(3, 0b01011000) == "");
    REQUIRE(best_repr(3, 0b01011001) == "");
    REQUIRE(best_repr(3, 0b01011010) == "");
    REQUIRE(best_repr(3, 0b01011011) == "");
    REQUIRE(best_repr(3, 0b01011100) == "");
    REQUIRE(best_repr(3, 0b01011101) == "");
    REQUIRE(best_repr(3, 0b01011110) == "");
    REQUIRE(best_repr(3, 0b01011111) == "");
    REQUIRE(best_repr(3, 0b01100000) == "");
    REQUIRE(best_repr(3, 0b01100001) == "");
    REQUIRE(best_repr(3, 0b01100010) == "");
    REQUIRE(best_repr(3, 0b01100011) == "");
    REQUIRE(best_repr(3, 0b01100100) == "");
    REQUIRE(best_repr(3, 0b01100101) == "");
    REQUIRE(best_repr(3, 0b01100110) == "");
    REQUIRE(best_repr(3, 0b01100111) == "");
    REQUIRE(best_repr(3, 0b01101000) == "");
    REQUIRE(best_repr(3, 0b01101001) == "");
    REQUIRE(best_repr(3, 0b01101010) == "");
    REQUIRE(best_repr(3, 0b01101011) == "");
    REQUIRE(best_repr(3, 0b01101100) == "");
    REQUIRE(best_repr(3, 0b01101101) == "");
    REQUIRE(best_repr(3, 0b01101110) == "");
    REQUIRE(best_repr(3, 0b01101111) == "");
    REQUIRE(best_repr(3, 0b01110000) == "");
    REQUIRE(best_repr(3, 0b01110001) == "");
    REQUIRE(best_repr(3, 0b01110010) == "");
    REQUIRE(best_repr(3, 0b01110011) == "");
    REQUIRE(best_repr(3, 0b01110100) == "");
    REQUIRE(best_repr(3, 0b01110101) == "");
    REQUIRE(best_repr(3, 0b01110110) == "");
    REQUIRE(best_repr(3, 0b01110111) == "");
    REQUIRE(best_repr(3, 0b01111000) == "");
    REQUIRE(best_repr(3, 0b01111001) == "");
    REQUIRE(best_repr(3, 0b01111010) == "");
    REQUIRE(best_repr(3, 0b01111011) == "");
    REQUIRE(best_repr(3, 0b01111100) == "");
    REQUIRE(best_repr(3, 0b01111101) == "");
    REQUIRE(best_repr(3, 0b01111110) == "");
    REQUIRE(best_repr(3, 0b01111111) == "");
    REQUIRE(best_repr(3, 0b10000000) == "");
    REQUIRE(best_repr(3, 0b10000001) == "");
    REQUIRE(best_repr(3, 0b10000010) == "");
    REQUIRE(best_repr(3, 0b10000011) == "");
    REQUIRE(best_repr(3, 0b10000100) == "");
    REQUIRE(best_repr(3, 0b10000101) == "");
    REQUIRE(best_repr(3, 0b10000110) == "");
    REQUIRE(best_repr(3, 0b10000111) == "");
    REQUIRE(best_repr(3, 0b10001000) == "");
    REQUIRE(best_repr(3, 0b10001001) == "");
    REQUIRE(best_repr(3, 0b10001010) == "");
    REQUIRE(best_repr(3, 0b10001011) == "");
    REQUIRE(best_repr(3, 0b10001100) == "");
    REQUIRE(best_repr(3, 0b10001101) == "");
    REQUIRE(best_repr(3, 0b10001110) == "");
    REQUIRE(best_repr(3, 0b10001111) == "");
    REQUIRE(best_repr(3, 0b10010000) == "");
    REQUIRE(best_repr(3, 0b10010001) == "");
    REQUIRE(best_repr(3, 0b10010010) == "");
    REQUIRE(best_repr(3, 0b10010011) == "");
    REQUIRE(best_repr(3, 0b10010100) == "");
    REQUIRE(best_repr(3, 0b10010101) == "");
    REQUIRE(best_repr(3, 0b10010110) == "");
    REQUIRE(best_repr(3, 0b10010111) == "");
    REQUIRE(best_repr(3, 0b10011000) == "");
    REQUIRE(best_repr(3, 0b10011001) == "");
    REQUIRE(best_repr(3, 0b10011010) == "");
    REQUIRE(best_repr(3, 0b10011011) == "");
    REQUIRE(best_repr(3, 0b10011100) == "");
    REQUIRE(best_repr(3, 0b10011101) == "");
    REQUIRE(best_repr(3, 0b10011110) == "");
    REQUIRE(best_repr(3, 0b10011111) == "");
    REQUIRE(best_repr(3, 0b10100000) == "");
    REQUIRE(best_repr(3, 0b10100001) == "");
    REQUIRE(best_repr(3, 0b10100010) == "");
    REQUIRE(best_repr(3, 0b10100011) == "");
    REQUIRE(best_repr(3, 0b10100100) == "");
    REQUIRE(best_repr(3, 0b10100101) == "");
    REQUIRE(best_repr(3, 0b10100110) == "");
    REQUIRE(best_repr(3, 0b10100111) == "");
    REQUIRE(best_repr(3, 0b10101000) == "");
    REQUIRE(best_repr(3, 0b10101001) == "");
    REQUIRE(best_repr(3, 0b10101010) == "");
    REQUIRE(best_repr(3, 0b10101011) == "");
    REQUIRE(best_repr(3, 0b10101100) == "");
    REQUIRE(best_repr(3, 0b10101101) == "");
    REQUIRE(best_repr(3, 0b10101110) == "");
    REQUIRE(best_repr(3, 0b10101111) == "");
    REQUIRE(best_repr(3, 0b10110000) == "");
    REQUIRE(best_repr(3, 0b10110001) == "");
    REQUIRE(best_repr(3, 0b10110010) == "");
    REQUIRE(best_repr(3, 0b10110011) == "");
    REQUIRE(best_repr(3, 0b10110100) == "");
    REQUIRE(best_repr(3, 0b10110101) == "");
    REQUIRE(best_repr(3, 0b10110110) == "");
    REQUIRE(best_repr(3, 0b10110111) == "");
    REQUIRE(best_repr(3, 0b10111000) == "");
    REQUIRE(best_repr(3, 0b10111001) == "");
    REQUIRE(best_repr(3, 0b10111010) == "");
    REQUIRE(best_repr(3, 0b10111011) == "");
    REQUIRE(best_repr(3, 0b10111100) == "");
    REQUIRE(best_repr(3, 0b10111101) == "");
    REQUIRE(best_repr(3, 0b10111110) == "");
    REQUIRE(best_repr(3, 0b10111111) == "");
    REQUIRE(best_repr(3, 0b11000000) == "");
    REQUIRE(best_repr(3, 0b11000001) == "");
    REQUIRE(best_repr(3, 0b11000010) == "");
    REQUIRE(best_repr(3, 0b11000011) == "");
    REQUIRE(best_repr(3, 0b11000100) == "");
    REQUIRE(best_repr(3, 0b11000101) == "");
    REQUIRE(best_repr(3, 0b11000110) == "");
    REQUIRE(best_repr(3, 0b11000111) == "");
    REQUIRE(best_repr(3, 0b11001000) == "");
    REQUIRE(best_repr(3, 0b11001001) == "");
    REQUIRE(best_repr(3, 0b11001010) == "");
    REQUIRE(best_repr(3, 0b11001011) == "");
    REQUIRE(best_repr(3, 0b11001100) == "");
    REQUIRE(best_repr(3, 0b11001101) == "");
    REQUIRE(best_repr(3, 0b11001110) == "");
    REQUIRE(best_repr(3, 0b11001111) == "");
    REQUIRE(best_repr(3, 0b11010000) == "");
    REQUIRE(best_repr(3, 0b11010001) == "");
    REQUIRE(best_repr(3, 0b11010010) == "");
    REQUIRE(best_repr(3, 0b11010011) == "");
    REQUIRE(best_repr(3, 0b11010100) == "");
    REQUIRE(best_repr(3, 0b11010101) == "");
    REQUIRE(best_repr(3, 0b11010110) == "");
    REQUIRE(best_repr(3, 0b11010111) == "");
    REQUIRE(best_repr(3, 0b11011000) == "");
    REQUIRE(best_repr(3, 0b11011001) == "");
    REQUIRE(best_repr(3, 0b11011010) == "");
    REQUIRE(best_repr(3, 0b11011011) == "");
    REQUIRE(best_repr(3, 0b11011100) == "");
    REQUIRE(best_repr(3, 0b11011101) == "");
    REQUIRE(best_repr(3, 0b11011110) == "");
    REQUIRE(best_repr(3, 0b11011111) == "");
    REQUIRE(best_repr(3, 0b11100000) == "");
    REQUIRE(best_repr(3, 0b11100001) == "");
    REQUIRE(best_repr(3, 0b11100010) == "");
    REQUIRE(best_repr(3, 0b11100011) == "");
    REQUIRE(best_repr(3, 0b11100100) == "");
    REQUIRE(best_repr(3, 0b11100101) == "");
    REQUIRE(best_repr(3, 0b11100110) == "");
    REQUIRE(best_repr(3, 0b11100111) == "");
    REQUIRE(best_repr(3, 0b11101000) == "");
    REQUIRE(best_repr(3, 0b11101001) == "");
    REQUIRE(best_repr(3, 0b11101010) == "");
    REQUIRE(best_repr(3, 0b11101011) == "");
    REQUIRE(best_repr(3, 0b11101100) == "");
    REQUIRE(best_repr(3, 0b11101101) == "");
    REQUIRE(best_repr(3, 0b11101110) == "");
    REQUIRE(best_repr(3, 0b11101111) == "");
    REQUIRE(best_repr(3, 0b11110000) == "");
    REQUIRE(best_repr(3, 0b11110001) == "");
    REQUIRE(best_repr(3, 0b11110010) == "");
    REQUIRE(best_repr(3, 0b11110011) == "");
    REQUIRE(best_repr(3, 0b11110100) == "");
    REQUIRE(best_repr(3, 0b11110101) == "");
    REQUIRE(best_repr(3, 0b11110110) == "");
    REQUIRE(best_repr(3, 0b11110111) == "");
    REQUIRE(best_repr(3, 0b11111000) == "");
    REQUIRE(best_repr(3, 0b11111001) == "");
    REQUIRE(best_repr(3, 0b11111010) == "");
    REQUIRE(best_repr(3, 0b11111011) == "");
    REQUIRE(best_repr(3, 0b11111100) == "");
    REQUIRE(best_repr(3, 0b11111101) == "");
    REQUIRE(best_repr(3, 0b11111110) == "");
    REQUIRE(best_repr(3, 0b11111111) == "t");
}

// TEST_CASE("Random simplifications", "[simpl]") {
//     REQUIRE(simplify("nA or AnBC") == "nA or nBC");
//     REQUIRE(simplify("nAD or nBD or ABC") == "nAD or nBD or ABC");
//     REQUIRE(simplify("ABC or nAC or nBC") == "C");
//     REQUIRE(simplify("nAB or nBC or nAC") == "nAB or nBC");
//     REQUIRE(simplify("nAC or nAB or BC") == "nAC or nAB or BC");
//     REQUIRE(simplify("n(AB(A or nBC) or n(n(AC) and n(BC)))") == "nAnB or nAnC or nBnC");
//     REQUIRE(simplify("BnCnD or AC or AnD or AnB") == "BnCnD or AC or AnB");
// }
