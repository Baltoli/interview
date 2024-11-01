#pragma once

#include <cstdint>
#include <expected>
#include <tuple>
#include <vector>

namespace vm
{
    enum class Opcode : uint8_t
    {
        STOP = 0x00,
        PUSH = 0x01,
        POP = 0x02,
        JUMPDEST = 0x03,
        ADD = 0x04,
        ISZERO = 0x05,
        PRINT = 0x06,
        JUMPI = 0x07,
        EQUAL = 0x08,

        INVALID,
    };

    struct Instruction
    {
        Opcode op;
        uint64_t data;
        std::size_t pc;
    };

    bool operator==(Instruction const &a, Instruction const &b);

    enum class ParseError
    {
        OddNumberOfBytes,
        InvalidByte,
        UnknownOpcode,
        InvalidPushData,
    };

    std::expected<std::vector<Instruction>, ParseError>
    parse_instructions(std::vector<char> const &);
}
