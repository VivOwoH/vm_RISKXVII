#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

// byte addressable
#define INSTRUC_MEM 1024
#define DATA_MEM 1024
#define VROUTINE_MEM 255
#define NUM_BANK 128 
#define BANK_SZ 64
#define HEAP_MEM NUM_BANK * BANK_SZ // 128 banks of 64 bytes
#define HEAP_START_ADDR 0xb700

#define NUM_REG 33

// banks are consecutive, connected as a linked list (no idx)
struct heap_bank {
    int is_free;
    uint32_t addr; // starting addr
    int alloc_len; // how many virtual addr does it map to
    struct heap_bank * prev; // prev bank
    struct heap_bank * next; // next bank
    uint8_t bank_content[BANK_SZ]; // actual malloc mem
};

extern struct heap_bank * heap[NUM_BANK];

struct mem {
    uint8_t inst_data_mem[INSTRUC_MEM + DATA_MEM];
    uint8_t vroutine_mem[VROUTINE_MEM];
    struct heap_bank heap_mem[NUM_BANK];
};

extern struct mem * memptr;

// REGISTERS
enum { R0 = 0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12,
       R13, R14, R15, R16, R17, R18, R19, R20, R21, R22, R23,
       R24, R25, R26, R27, R28, R29, R30, R31, RPC };

extern uint32_t regs[NUM_REG]; // separated from mem for easier access

// VIRTUAL ROUTINES
enum {
   C_Write_char = 0x0800,
   C_Write_int = 0x0804,
   C_Write_uint = 0x0808,
   Halt = 0x080C,
   C_Read_char = 0x0812,
   C_Read_int = 0x0816,
   D_PC = 0x0820,
   D_reg_bank = 0x0824,
   D_mem_word = 0x0828,
   H_malloc = 0x0830,
   H_free = 0x0834 
};

#endif