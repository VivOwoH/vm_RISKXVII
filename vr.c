#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "vr.h"
#include "types.h"

// print value stored as a single ASCII encoded character to stdout
void Console_Write_char(uint32_t value) {
    fprintf(stdout, "%c", value);
}

// print the value being stored as a single 32-bit signed int in decimal format to stdout
void Console_Write_int(uint32_t value) {
    fprintf(stdout, "%d", value);
}

// print the value being stored as a single 32-bit (4bytes) unsigned int in lower case hexadecimal format to stdout
void Console_Write_uint(uint32_t value) {
    fprintf(stdout, "%.x", value);
}

// halt and exit
void Console_Halt() {
    fprintf(stdout, "CPU Halt Requested\n");
    exit(3);
}

// scan input from stdin and treat the input as an ASCII-encoded character for the memory load result
uint32_t Console_Read_char() {
    char c;
    scanf("%c", &c);
    return c;
}

// scan input from stdin and parse the input as a signed integer for the memory load result
uint32_t Console_Read_int() {
    int n;
    scanf("%d", &n);
    return n;
}

// print the value of PC in lower case hexadecimal format to stdout
void Dump_PC() {
    fprintf(stdout, "%.x\n", regs[RPC]);
}

// print the value of all registers, including PC, in lower case hexadecimal format to stdout
// e.g. R[0] = 0xffffffff;
void Dump_reg_bank() {
    fprintf(stdout, "PC = 0x%.8x;\n", regs[RPC]);
    for (int i = 0; i < NUM_REG-1; i++) { // 33 reg exclude pc
        fprintf(stdout, "R[%d] = 0x%.8x;\n", i, regs[i]);
    }
}

// print the value of M[v] in lower case hexadecimal format to stdout 
// v is the value being stored interpreted as an 32-bit unsigned integer
void Dump_mem_word(uint32_t v, uint32_t instruc) {
    if (v > (INSTRUC_MEM + DATA_MEM)) {
        err_illegal_op(instruc);
    } else {
        fprintf(stdout, "%.x\n", memptr->inst_mem[v]);
    }
}

void err_not_implemented(uint32_t instruc) {
    printf("Instruction Not Implemented: 0x%.8x\n", instruc);
    Dump_reg_bank();
    exit(4);
}

void err_illegal_op(uint32_t instruc) {
    printf("Illegal Operation: 0x%.8x\n", instruc);
    Dump_reg_bank();
    exit(5);
}