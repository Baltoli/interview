#include "instructions.hpp"
#include "vm.hpp"

#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

[[noreturn]] void usage(std::string_view executable)
{
    std::cerr << std::format("usage: {} FILE\n", executable);
    std::exit(1);
}

std::vector<char> read_file(fs::path const &p)
{
    auto in = std::ifstream(p);

    auto all_chars = std::vector<char>(std::istreambuf_iterator<char>(in), {});
    auto hex_chars = std::vector<char>{};

    std::copy_if(
        all_chars.begin(),
        all_chars.end(),
        std::back_inserter(hex_chars),
        [](auto c) { return !std::isspace(c); });

    return hex_chars;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        usage(argv[0]);
    }

    auto input_file = fs::path(argv[1]);

    if (!fs::is_regular_file(input_file)) {
        usage(argv[0]);
    }

    auto code = read_file(input_file);
    auto parse_result = vm::parse_instructions(code);

    if (!parse_result.has_value()) {
        std::cerr << "Parse error!\n";
        std::exit(1);
    }

    auto result = vm::execute(parse_result.value());
    std::cout << (result ? "success" : "failure") << '\n';
}
