#include <stdint.h>

// byte addressable
#define INSTRUC_MEM 1024
#define DATA_MEM 1024
#define VROUTINE_MEM 255
#define HEAP 128 * 64

#define NUM_REG 33

struct mem {
    uint8_t inst_mem[INSTRUC_MEM];
    uint8_t data_mem[DATA_MEM];
    uint8_t vroutine_mem[VROUTINE_MEM];
};

extern struct mem * memptr;

// REGISTERS
enum { R0 = 0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12,
       R13, R14, R15, R16, R17, R18, R19, R20, R21, R22, R23,
       R24, R25, R26, R27, R28, R29, R30, R31, RPC };

extern uint32_t regs[NUM_REG]; 

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
   D_mem_word = 0x0828
};