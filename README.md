# Interview Compiler

This project implements a compiler and runtime for a tiny stack-based
programming language similar to the one you implemented in your take-home
exercise; the compiler directly targets X86 without using an intermediate
representation.

## Language Spec

Programs in this language execute on a stack-based abstract machine. Entries on
the stack are 64-bit unsigned integers. Programs exit with either success or
failure, and can interact with the outside host environment through the `PRINT`
opcode. The language implemented supports 9 opcodes, each encoded as a single
byte:
- `STOP`(`0x00`): end execution immediately with a success code.
- `PUSH`(`0x01`): push the 8 bytes following this instruction to the stack.
- `POP`(`0x02`): pop an entry off the stack.
- `JUMPDEST`(`0x03`): mark a valid jump destination.
- `ADD`(`0x04`): pop two values from the stack, and push their wrapping sum.
- `ISZERO`(`0x05`): pop an entry from the stack, and push `1` if it was zero,
  and `0` otherwise.
- `PRINT`(`0x06`): pop the top entry from the stack and print it to standard
  output.
- `JUMPI`(`0x07`): pop two entries from the stack, and jump to the program
  counter at the first entry if the second is non-zero. Exit with failure if the
  destination is not a `JUMPDEST` instruction.
- `EQUAL`(`0x08`): pop two entries from the stack, and push `1` if they are
  equal, and `0` if they are not.

The `PUSH` opcode is always followed by precisely 8 bytes of additional data.

## Interview Exercise

The aim of this interview is to get a sense of how you work on "real" code in a
pair-programming scenario. The code in this repo is intentionally left in quite
an imperfect state in order to prompt discussion during the interview; there are
bugs in it (intentional and otherwise), and it lacks in-code documentation.

Over the course of the interview, I'd like to try and implement some small
features on top of the existing code. Some ideas are:
- Subtraction.
- Stack manipulation opcodes (`SWAP` or `DUP`).
- More host features (persistent storage?).

The solution we reach in an hour won't be perfect or necessarily even complete,
but the aim is to get a sense of how you work on unfamiliar code and prioritise
when under a degree of time pressure.

## Getting Started

```console
$ cmake --build build
$ ./build/monad-run test.in
$ cat /tmp/asm.s # debugging
```