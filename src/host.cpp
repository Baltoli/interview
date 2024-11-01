#include "host.hpp"

#include <iostream>

namespace vm
{
    Host::Host(Host::print_fn_t p)
        : print_impl_(p)
    {
    }

    void Host::print(uint64_t val)
    {
        print_impl_(val);
    }
}
