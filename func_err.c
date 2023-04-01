#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "func_err.h"
#include "types.h"

// ------------------- MEMORY LOAD/STORE -----------------------

uint32_t mem_read(uint32_t addr, int num_cell, uint32_t instruc) {
    switch (addr) {
        case (C_Read_char):
            // puts("-------console read char-------"); 
            return Console_Read_char();
        case (C_Read_int):
            // puts("---------console read int-------"); 
            return Console_Read_int();
        default:
            // read from instruc or data allowed (up to 4 bytes)
            if (addr < (INSTRUC_MEM + DATA_MEM)) {
                uint32_t result = 0;
                for (int i = 0; i < num_cell; i++) {
                    if (addr + i >= INSTRUC_MEM + DATA_MEM) // cells outside range
                        err_illegal_op(instruc);
                    result += memptr->inst_data_mem[addr+i] << (8 * (num_cell-1-i)); // e.g. 32bits:8*3, 8*2, 8*1, 8*0
                }
                return result;
            }
            // read from heap bank: find heap bank that has addr in its range to check alloc
            // NOTE: can read up to 4 bytes
            else if (addr >= HEAP_START_ADDR && (addr+num_cell-1) < (HEAP_START_ADDR + HEAP_MEM)) {
                return heap_read_write(addr, num_cell, instruc, 0, LOAD);
            } 
            else {
                err_illegal_op(instruc);
            }
    }
    return 0;
}

uint32_t mem_write(uint32_t addr, uint32_t value, int num_cell, uint32_t instruc) {
    switch (addr) {
        case (C_Write_char):
            // puts("---------console write char-------"); 
            Console_Write_char(value);
            break;
        case (C_Write_int):
            // puts("---------console write int-------"); 
            Console_Write_int(value);
            break;
        case (C_Write_uint):
            // puts("---------console write uint-------"); 
            Console_Write_uint(value);
            break;
        case (Halt):
            Console_Halt();
            break;
        case (D_PC):
            // puts("---------dump pc-------\n"); 
            Dump_PC();
            break;
        case (D_reg_bank):
            // puts("---------dump reg bank-------\n"); 
            Dump_reg_bank();
            break;
        case (D_mem_word):
            // puts("---------dump mem word-------\n"); 
            Dump_mem_word(value, instruc);
            break;
        case (H_malloc):
            // puts("---------malloc-------\n");
            regs[R28] = VM_malloc(value);
            break;
        case (H_free):
            // puts("---------free-------\n");
            VM_free(value, instruc);
            break;
        default:
            // write to data allowed, not write to instruc
            if (addr >= INSTRUC_MEM && addr < (INSTRUC_MEM + DATA_MEM)) {
                for (int i = 0; i < num_cell; i++) {
                    if (addr + i >= INSTRUC_MEM + DATA_MEM) // cells outside range
                        err_illegal_op(instruc);
                    memptr->inst_data_mem[addr+i] = ( value >> (8 * (num_cell-1-i)) ) & 0xFF; 
                }
            }
            // write to heap bank: find heap bank that has addr in its range to check alloc
            // NOTE: can write up to 4 bytes
            else if (addr >= HEAP_START_ADDR && (addr+num_cell-1) < (HEAP_START_ADDR + HEAP_MEM)) {
                return heap_read_write(addr, num_cell, instruc, value, STORE);
            } 
            else {
                err_illegal_op(instruc);
            }            
            break;
    }
    return 0;
}


// -------------------- Virtual Routines ----------------------
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

// print the value of M[v] (1-byte) in lower case hexadecimal format to stdout 
// v is the value being stored interpreted as an 4-bytes unsigned integer
void Dump_mem_word(uint32_t v, uint32_t instruc) {
    if (v < (INSTRUC_MEM + DATA_MEM)) {
        fprintf(stdout, "%x\n", memptr->inst_data_mem[v]);
    } else if (v >= HEAP_START_ADDR && v < HEAP_START_ADDR + HEAP_MEM) {
        fprintf(stdout, "%x\n", heap_read_write(v, 1, instruc, 0, 0));
    } else {
        err_illegal_op(instruc);
    }
}

// -------------------- Heap methods ----------------------
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

// helper function for load/store in heap
// load = 0; store = 1
// **Load does not need "value" (thus 0)
uint32_t heap_read_write(uint32_t addr, int num_cell, uint32_t instruc, uint32_t value, int mode) {
    for (int i = 0; i < NUM_BANK; i++) {   
        uint32_t heap_end_addr = heap[i]->addr + heap[i]->alloc_len - 1;

        // check if valid offset from malloced address, and bank is allocated
        // case 1: in 1 block
        uint32_t result = 0;

        if (addr >= heap[i]->addr && (addr+num_cell-1) <= heap_end_addr 
                && !heap[i]->is_free) {
            for (int j = 0; j < num_cell; j++) {
                if (mode == LOAD) {
                    result += heap[i]->bank_content[addr - heap[i]->addr + j] 
                            << (8 * (num_cell-1-j)); // e.g. 32bits:8*3, 8*2, 8*1, 8*0
                } 
                else if (mode == STORE) {
                    heap[i]->bank_content[addr - heap[i]->addr + j] 
                            = ( value >> (8 * (num_cell-1-j)) ) & 0xFF; 
                }
            }
            return result;
        } 
        // case 2: in multiple blocks
        else if (addr >= heap[i]->addr && (addr <= heap_end_addr) && (addr+num_cell-1) > heap_end_addr
                && !heap[i]->is_free && !heap[i+1]->is_free) {

            int overflow = (addr+num_cell-1) - heap_end_addr;

            for (int j = 0; j < (num_cell - overflow); j++) {
                if (mode == LOAD) {
                    result += heap[i]->bank_content[addr - heap[i]->addr + j] 
                            << (8 * (num_cell-1-j)); // e.g. 32bits:8*3, 8*2, 8*1, 8*0
                }
                else if (mode == STORE) {
                    heap[i]->bank_content[addr - heap[i]->addr + j] 
                            = ( value >> (8 * (num_cell-1-j)) ) & 0xFF; 
                }
            }
            // overflow -> next block in heap
            for (int k = 0; k < overflow; k++) {
                if (mode == LOAD) {
                    result += heap[i+1]->bank_content[heap[i+1]->addr + k] 
                        << (8 * (overflow - 1 - k)); // e.g. 32bits:8*3, 8*2, 8*1, 8*0
                }
                else if (mode == STORE) {
                    heap[i+1]->bank_content[heap[i+1]->addr + k] 
                        = ( value >> (8 * (overflow - 1 - k)) ) & 0xFF; 
                }
            }
            return result;
        } 
    }
    err_illegal_op(instruc); // reaching here means cannot find valid bank(s)
    return 0; // error
}

// -------------------- Error handling ----------------------
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