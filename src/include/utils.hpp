#ifndef UTILS_HPP
#define UTILS_HPP

#include <ranges>

namespace stdr = std::ranges;
namespace stdv = std::views;
using namespace std::literals;

template <typename... Ts>
struct overloads : Ts... {
    using Ts::operator()...;
};

auto bits(unsigned val, std::ptrdiff_t cnt) {
    return stdv::iota(0, cnt) | stdv::reverse | stdv::transform([val](int i) -> bool { return (val >> i) & 1; });
}

#endif