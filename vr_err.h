#ifndef VR_H
#define VR_H

#include <stdint.h>

// virtual routines
void Console_Write_char(uint32_t);
void Console_Write_int(uint32_t);
void Console_Write_uint(uint32_t);
void Console_Halt();
uint32_t Console_Read_char();
uint32_t Console_Read_int();
void Dump_PC();
void Dump_reg_bank();
void Dump_mem_word(uint32_t, uint32_t);
void VM_malloc(uint32_t);
void VM_free(uint32_t, uint32_t);

// helper function
struct heap_bank * check_valid_addr(uint32_t);

// error handling
void err_illegal_op(uint32_t);
void err_not_implemented(uint32_t);

#endif