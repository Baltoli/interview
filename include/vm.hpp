#pragma once

#include "instructions.hpp"

#include <cstdint>

extern "C"
{
void vm_print(void *, uint64_t);
}

namespace vm
{
    bool execute(std::vector<Instruction> const &);
}
