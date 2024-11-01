#pragma once

#include <cstdint>

namespace vm
{
    class Host
    {
    public:
        using print_fn_t = void (*)(uint64_t);

        explicit Host(print_fn_t print_);

        void print(uint64_t);

    private:
        print_fn_t print_impl_;
    };
}
