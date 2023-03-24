- 0x0000 - 0x3ff: Instruction Memory - Contains 2^10 of bytes for text segment. (1024 bytes)

- 0x0400 - 0x7ff: Data Memory - Contains 2^10 of bytes for global variables, and function stack. (1024 bytes)

- 0x0800 - 0x8ff: Virtual Routines - Accesses to these address will cause special operations to be called. (255 bytes)

- 0xb700 +: Heap Banks - Hardware managed 128 x 64 bytes banks of dynamically allocate-able memory.


## Memory
total memory 10495 bytes?

memory[]


## Registers
32 general purpose registers (each 4 bytes)
- 1st one is zero register
1 program counter (also 4 bytes)

reg[33]


## Instructions

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
- type U: 0110111 = 37
- type S: 0100011 = 23
- type SB: 1100011 = 63
- type UJ: 1101111 = 6F

32 bits instruction with first 6 bits opcode
(can right shift [divide by 2] 26 bits to get opcode)
right shift by different bits to get registers, imm etc.





