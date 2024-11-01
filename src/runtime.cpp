#include "runtime.hpp"

#include <cstdint>
#include <format>
#include <iostream>

extern "C"
{
void runtime_print(uint64_t value)
{
    runtime::print_to(std::cout, value);
    std::cout << '\n';
}
}

namespace runtime
{
    void print_to(std::ostream &os, uint64_t value)
    {
        os << std::format("0x{:016X}", value);
    }
}
