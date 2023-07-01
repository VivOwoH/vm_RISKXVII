# Virtual Machine (RISC-V)
## Testcases
- basic arithmetic (with func) ✓
- program involves branching ✓
- invoke all virtual routines 
- heap malloc
- heap free
- heap free middle bank & malloc again

## Edge cases:
- Read/Write data[1023] (OK)
- Read/Write data[1024] (NO)
- Read/Write data[1023], 2 bytes (NO)
- Heap read/write 1 byte (OK)
- Heap read/write 4 bytes (OK)
- Heap read/write 4 bytes across banks (OK)
- Heap read/write freed/unallocated (NO)
- Heap free not start address (NO)
- Heap free read/write/free address out of range (NO)

## Exit codes
1 - invalid command-line argument provided
2 - cannot open image file specified
3 - normal halt operation
4 - instruction not implemented
5 - illegal operation (all invalid memory accesses trigger this)
6 - Reject file larger than expected size (2048 bytes)

## Memory
- 0x0000 - 0x3ff: Instruction Memory - Contains 2^10 of bytes for text segment. (1024 bytes)
- 0x0400 - 0x7ff: Data Memory - Contains 2^10 of bytes for global variables, and function stack. (1024 bytes)
- 0x0800 - 0x8ff: Virtual Routines - Accesses to these address will cause special operations to be called. (255 bytes)
- 0xb700 +: 128 Heap Banks including meta information, and a 64-bytes array of dynamically allocate-able memory

## Registers
32 general purpose registers + 1PC (each 4 bytes)
- R0 maintains 0
- R1~R31: general purpose
- R32 = PC 

## Instructions
*32 bits instruction with right most 7 bits as opcode*

[Arithmetic and Logic]
- type R: 0110011
- type I: 0010011
- type U: 0110111

[Memory access]
- type I: 0000011
- tyype S: 0100011

[Program Flow Operations]
- type R: 0110011
- type I: 0010011 / 1100111
- type SB: 1100011
- type UJ: 1101111

Summary:
- type R: 0110011 = 33
- type I: 0010011 = 13 / 0000011 = 3/ 1100111 = 67
- type S: 0100011 = 23
- type SB: 1100011 = 63
- type U: 0110111 = 37 [lui]
- type UJ: 1101111 = 6F [jal]

## Flags used to reduce executable size
-Os: optimize size
-s: strip the binary
-fno-ident: no compiler name and version information
-fno-exceptions: omit code to proprogate exception handling
-fno-asynchronous-unwind-tables: omit static unwind table
