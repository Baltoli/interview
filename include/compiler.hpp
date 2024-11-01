#pragma once

#include "instructions.hpp"

#include <array>
#include <filesystem>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

namespace vm
{
    class Compiler
    {
    public:
        using program_entrypoint_t = void (*)(uint64_t *, void *);

        explicit Compiler(std::vector<Instruction>);

        program_entrypoint_t get_entrypoint();

    private:
        static constexpr auto entrypoint_symbol = "entrypoint";

        static constexpr auto exit_block_label = ".exit";
        static constexpr auto success_block_label = ".success";
        static constexpr auto failure_block_label = ".failure";
        static constexpr auto jump_table_label = ".jump_table";

        static constexpr auto reg_host_ptr = "rbx";
        static constexpr auto reg_success_ptr = "r12";

        static constexpr auto reg_scratch =
            std::array{"rax", "rdi", "rsi", "rdx", "rcx", "r8", "r9"};

        std::vector<Instruction> instructions_;

        void emit_prelude(std::ostream &);
        void emit_instruction(std::ostream &, Instruction const &);
        void emit_postlude(std::ostream &);
        void emit_jump_table(std::ostream &);

        fs::path compile_to_asm();
        fs::path link_to_shared_library(fs::path const &);
    };
}
