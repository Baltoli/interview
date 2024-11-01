#include "compiler.hpp"
#include "instructions.hpp"

#include <cstdlib>
#include <dlfcn.h>
#include <format>
#include <fstream>
#include <iostream>

template <typename... Args>
auto asm_(std::string_view format, Args const &...args)
{
    return std::format(
        "\t{}\n", std::vformat(format, std::make_format_args(args...)));
}

namespace vm
{
    Compiler::Compiler(std::vector<Instruction> insts)
        : instructions_(std::move(insts))
    {
    }

    Compiler::program_entrypoint_t Compiler::get_entrypoint()
    {
        auto asm_path = compile_to_asm();
        auto shlib_path = link_to_shared_library(asm_path);

        auto handle = dlopen(shlib_path.c_str(), RTLD_NOW | RTLD_GLOBAL);
        if (!handle) {
            std::cerr << dlerror() << '\n';
            return nullptr;
        }

        auto fn = dlsym(handle, entrypoint_symbol);
        if (!fn) {
            std::cerr << dlerror() << '\n';
            return nullptr;
        }

        return reinterpret_cast<program_entrypoint_t>(fn);
    }

    void Compiler::emit_prelude(std::ostream &os)
    {
        os << ".section .note.GNU-stack,\"\",@progbits\n"
           << ".text\n"
           << std::format(".globl {}\n", entrypoint_symbol)
           << ".p2align 4, 0x90\n"
           << std::format(".type {},@function\n", entrypoint_symbol)
           << std::format("{}:\n", entrypoint_symbol);

        os << asm_("pushq %{}", reg_host_ptr);
        os << asm_("pushq %{}", reg_success_ptr);
        os << asm_("push %rbp");
        os << asm_("mov %rsp, %rbp");
        os << asm_("movq %rdi, %{}", reg_success_ptr);
        os << asm_("movq %rsi, %{}", reg_host_ptr);
    }

    void Compiler::emit_instruction(std::ostream &os, Instruction const &inst)
    {
        switch (inst.op) {
        case Opcode::STOP:
            os << asm_("jmp {}", success_block_label);
            break;
        case Opcode::PUSH:
            os << asm_("pushq ${}", inst.data);
            break;
        case Opcode::POP:
            os << asm_("addq $8, %rsp");
            break;
        case Opcode::JUMPDEST:
            os << std::format(".block_{}:\n", inst.pc);
            break;
        case Opcode::ADD:
            os << asm_("popq %{}", reg_scratch[0]);
            os << asm_("popq %{}", reg_scratch[1]);
            os << asm_("addq %{}, %{}", reg_scratch[0], reg_scratch[1]);
            os << asm_("pushq %{}", reg_scratch[1]);
            break;
        case Opcode::ISZERO:
            os << asm_("popq %{}", reg_scratch[0]);
            os << asm_("testq %{0}, %{0}", reg_scratch[0]);
            os << asm_("pushq $0", reg_scratch[0]);
            os << asm_("setz (%rsp)", reg_scratch[0]);
            break;
        case Opcode::PRINT:
            os << asm_("movq %{}, %rdi", reg_host_ptr);
            os << asm_("popq %rsi");
            os << asm_("call vm_print@PLT");
            break;
        case Opcode::JUMPI:
            os << asm_("popq %{}", reg_scratch[0]);
            os << asm_("popq %{}", reg_scratch[1]);
            os << asm_("testq %{0}, %{0}", reg_scratch[1]);
            os << asm_("jz {}", jump_table_label);
            break;
        case Opcode::EQUAL:
            os << asm_("popq %{}", reg_scratch[0]);
            os << asm_("popq %{}", reg_scratch[1]);
            os << asm_("cmp %{}, %{}", reg_scratch[0], reg_scratch[1]);
            os << asm_("pushq $0", reg_scratch[0]);
            os << asm_("sete (%rsp)");
            break;
        }
    }

    void Compiler::emit_postlude(std::ostream &os)
    {
        os << std::format("{}:\n", success_block_label);
        os << asm_("movq $1, (%{})", reg_success_ptr);
        os << asm_("jmp {}", exit_block_label);

        os << std::format("{}:\n", failure_block_label);
        os << asm_("movq $0, (%{})", reg_success_ptr);
        os << asm_("jmp {}", exit_block_label);

        os << std::format("{}:\n", exit_block_label);
        os << asm_("mov %rbp, %rsp");
        os << asm_("popq %rbp");
        os << asm_("popq %{}", reg_success_ptr);
        os << asm_("popq %{}", reg_host_ptr);
        os << asm_("ret");
    }

    void Compiler::emit_jump_table(std::ostream &os)
    {
        os << std::format("{}:\n", jump_table_label);
        for (auto const &inst : instructions_) {
            if (inst.op == Opcode::JUMPDEST) {
                os << asm_("cmp ${}, %{}", inst.pc, reg_scratch[0]);
                os << asm_("je .block_{}", inst.pc);
            }
        }
        os << asm_("jmp {}", failure_block_label);
    }

    fs::path Compiler::compile_to_asm()
    {
        auto out_path = fs::temp_directory_path() / "asm.s";
        auto os = std::ofstream(out_path);

        emit_prelude(os);

        for (auto const &inst : instructions_) {
            emit_instruction(os, inst);
        }

        emit_postlude(os);
        emit_jump_table(os);

        return out_path;
    }

    fs::path Compiler::link_to_shared_library(fs::path const &in_path)
    {
        auto object_path = fs::temp_directory_path() / "object.o";
        auto as_command =
            std::format("as {} -o {}", in_path.string(), object_path.string());
        std::system(as_command.c_str());

        auto out_path = fs::temp_directory_path() / "compiled.so";
        auto command = std::format(
            "clang {} -fuse-ld=lld -nostdlib -shared -o {}",
            object_path.string(),
            out_path.string());
        std::system(command.c_str());

        return out_path;
    }
}
