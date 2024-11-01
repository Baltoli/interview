#include "instructions.hpp"
#include "runtime.hpp"

#include <cassert>
#include <limits>
#include <sstream>

void runtime_print_tests()
{
    auto to_string = [](auto val) {
        auto os = std::ostringstream{};
        runtime::print_to(os, val);
        return os.str();
    };

    assert(to_string(0) == "0x0000000000000000");
    assert(to_string(1) == "0x0000000000000001");
    assert(to_string(15) == "0x000000000000000F");
    assert(to_string(16) == "0x0000000000000010");
    assert(
        to_string(std::numeric_limits<uint32_t>::max()) ==
        "0x00000000FFFFFFFF");
    assert(
        to_string(std::numeric_limits<uint64_t>::max()) ==
        "0xFFFFFFFFFFFFFFFF");
}

void parse_tests()
{
    auto check_ok = [](std::string const &in,
                       std::vector<vm::Instruction> const &out) {
        auto vec_in = std::vector<char>{};
        std::copy(in.begin(), in.end(), std::back_inserter(vec_in));
        auto result = vm::parse_instructions(vec_in);
        assert(result.has_value());
        assert(result.value() == out);
    };

    check_ok("", {});
    check_ok("00", {{vm::Opcode::STOP, 0}});
    check_ok("06", {{vm::Opcode::PRINT, 0}});
    check_ok(
        "0002030405060708",
        {
            {vm::Opcode::STOP, 0},
            {vm::Opcode::POP, 0},
            {vm::Opcode::JUMPDEST, 0},
            {vm::Opcode::ADD, 0},
            {vm::Opcode::ISZERO, 0},
            {vm::Opcode::PRINT, 0},
            {vm::Opcode::JUMPI, 0},
            {vm::Opcode::EQUAL, 0},
        });

    check_ok(
        "01FFFFFFFFFFFFFFFF",
        {{vm::Opcode::PUSH, std::numeric_limits<uint64_t>::max()}});

    check_ok("010000000000000001", {{vm::Opcode::PUSH, 1}});
    check_ok("010000000000000100", {{vm::Opcode::PUSH, 256}});
    check_ok(
        "0401000000000000100102",
        {
            {vm::Opcode::ADD, 0},
            {vm::Opcode::PUSH, 4097},
            {vm::Opcode::POP, 0},
        });

    auto check_fail = [](std::string const &in, vm::ParseError err) {
        auto vec_in = std::vector<char>{};
        std::copy(in.begin(), in.end(), std::back_inserter(vec_in));
        auto result = vm::parse_instructions(vec_in);
        assert(!result.has_value());
        assert(result.error() == err);
    };

    check_fail("0", vm::ParseError::OddNumberOfBytes);
    check_fail("000", vm::ParseError::OddNumberOfBytes);
    check_fail("FF", vm::ParseError::UnknownOpcode);
    check_fail("10", vm::ParseError::UnknownOpcode);
    check_fail("FG", vm::ParseError::InvalidByte);
    check_fail("01000000000t000000", vm::ParseError::InvalidPushData);
}

int main()
{
    runtime_print_tests();
    parse_tests();
}
