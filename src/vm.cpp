#include "vm.hpp"
#include "compiler.hpp"
#include "host.hpp"
#include "runtime.hpp"

#include <iostream>

extern "C"
{
void vm_print(void *host, uint64_t val)
{
    static_cast<vm::Host *>(host)->print(val);
}
}

namespace vm
{
    bool execute(std::vector<Instruction> const &in)
    {
        auto compiler = Compiler(in);
        auto entry = compiler.get_entrypoint();

        auto success = uint64_t{0};
        auto host = Host(runtime_print);

        entry(&success, &host);

        return static_cast<bool>(success);
    }
}
