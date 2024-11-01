#pragma once

#include <cstdint>
#include <ostream>

extern "C"
{
void runtime_print(uint64_t);
}

namespace runtime
{
    void print_to(std::ostream &, uint64_t);
}
