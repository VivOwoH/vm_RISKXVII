#ifndef VR_H
#define VR_H

#define LOAD 0
#define STORE 1

#include <stdint.h>

// memory load/store helper
uint32_t mem_read(uint32_t addr, int num_cell, uint32_t instruc);
uint32_t mem_write(uint32_t addr, uint32_t value, int num_cell, uint32_t instruc);

// virtual routines
void Console_Write_char(uint32_t value);
void Console_Write_int(uint32_t value);
void Console_Write_uint(uint32_t value);
void Console_Halt();
uint32_t Console_Read_char();
uint32_t Console_Read_int();
void Dump_PC();
void Dump_reg_bank();
void Dump_mem_word(uint32_t v, uint32_t instruc);

// Heap
uint32_t VM_malloc(uint32_t value);
void VM_free(uint32_t addr, uint32_t instruc);
uint32_t heap_read_write(uint32_t addr, int num_cell, uint32_t instruc, uint32_t value, int mode);

// Error handling
void err_illegal_op(uint32_t instruc);
void err_not_implemented(uint32_t instruc);

#endif