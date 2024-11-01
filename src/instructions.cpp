#include "instructions.hpp"

#include <charconv>
#include <utility>

namespace vm
{
    std::expected<std::vector<Instruction>, ParseError>
    parse_instructions(std::vector<char> const &in)
    {
        auto result = std::vector<Instruction>{};

        if (in.size() % 2 != 0) {
            return std::unexpected(ParseError::OddNumberOfBytes);
        }

        for (auto it = in.begin(); it != in.end(); it += 2) {
            auto opcode_val = uint8_t{};

            auto first = &*it;
            auto last = &*(it + 2);

            auto conversion_result =
                std::from_chars(first, last, opcode_val, 16);

            if (conversion_result.ptr != last ||
                conversion_result.ec != std::errc{}) {
                return std::unexpected(ParseError::InvalidByte);
            }

            if (opcode_val >= std::to_underlying(Opcode::INVALID)) {
                return std::unexpected(ParseError::UnknownOpcode);
            }

            auto opcode = Opcode(opcode_val);
            auto data = uint64_t{0};

            if (opcode == Opcode::PUSH) {
                conversion_result = std::from_chars(last, last + 16, data, 16);

                if (conversion_result.ptr != last + 16 ||
                    conversion_result.ec != std::errc{}) {
                    return std::unexpected(ParseError::InvalidPushData);
                }

                it += 16;
            }

            result.emplace_back(opcode, data, it - in.begin());
        }

        return result;
    }

    bool operator==(Instruction const &a, Instruction const &b)
    {
        return std::tie(a.op, a.data) == std::tie(b.op, b.data);
    }
}
