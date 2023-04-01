#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "vr_err.h"
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
    fprintf(stdout, "%x", value);
}

// halt and exit
void Console_Halt() {
    fprintf(stdout, "CPU Halt Requested\n");
    exit(3);
}

// scan input from stdin and treat the input as an ASCII-encoded character for the memory load result
uint32_t Console_Read_char() {
    char c;
    if (scanf("%c", &c) != EOF)
        return c;
    return 0;
}

// scan input from stdin and parse the input as a signed integer for the memory load result
uint32_t Console_Read_int() {
    int n;
    if (scanf("%d", &n) != EOF)
        return n;
    return 0;
}

// print the value of PC in lower case hexadecimal format to stdout
void Dump_PC() {
    fprintf(stdout, "%x\n", regs[RPC]);
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
    if (v >= (INSTRUC_MEM + DATA_MEM)) {
        err_illegal_op(instruc);
    } else {
        fprintf(stdout, "%x\n", memptr->inst_data_mem[v]);
    }
}

void alloc_bank(struct heap_bank * bank, int len) {
    bank->is_free = 0;
    bank->alloc_len = len;
}

// request a bank with the size of the value being stored as len 
// ptr <- starting address(virtual); stored in R[28] <- 0 if can't allocate
uint32_t VM_malloc(uint32_t value) {
    // cannot malloc size if > virtual addr space
    if (value > HEAP_MEM) { 
        return 0;
    }

    // check if can allocate (consecutive free banks)
    int consecutives = ceil((double) value / BANK_SZ);
    int count = 0;
    for (int i = 0; i < NUM_BANK; i++) {
        count = (heap[i]->is_free) ? count + 1 : 0;
        
        // match consecutives required
        if (count == consecutives) {
            int start = i + 1 - consecutives;
            // alloc
            for (int j = 0; j < consecutives; j++) {
                if (j < consecutives - 1) { // link banks (e.g. 3 consec link 2 times)
                    // printf("%d, %d\n", start+j, start+j+1);
                    heap[start+j]->next = heap[start+j+1];
                    heap[start+j+1]->prev = heap[start+j];
                }
                int alloc_len = (value > BANK_SZ) ? BANK_SZ : value; // full partial alloc 
                alloc_bank(heap[start + j], alloc_len); 
                value -= alloc_len;
            }
            return heap[start]->addr; // update start addr
        }
    }
    return 0;
}

// free bank with specified STARTING addr as value being stored
// if addr was not allocated, an illegal operation error should be raised
void VM_free(uint32_t addr, uint32_t instruc) {
    for (int i = 0; i < NUM_BANK; i++) {
        // free bank if addr matched, starter/unlinked bank(no prev), and is allocated 
        if (heap[i]->addr == addr && heap[i]->prev == NULL && !heap[i]->is_free) {
            heap[i]->is_free = 1; // free current
            // free consec linked bank if exist
            struct heap_bank * current = heap[i]->next;
            while (current != NULL) {
                current->is_free = 1;
                current = current->next;
            }
            return;
        } 
    }
    err_illegal_op(instruc); // addr not found, raise error
    return;
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