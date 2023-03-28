#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
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

int current_bank_sz() {
    int size = 0;
    struct heap_bank * current = head_bank;
    while (current != NULL) {
        size++;
        current = current->next;
    }
    return size;
}

void alloc_bank(struct heap_bank * bank, struct heap_bank * prev_bank, int len) {
    struct heap_bank new_bank;
    if (bank == NULL) { // NULL ptr (create bank first)
        bank = &new_bank;
        prev_bank->next = bank; // update next ptr of prev bank
        // **Special case: specify tail bank
        tail_bank = (current_bank_sz() == NUM_BANK-1) ? bank : tail_bank;
    }

    if (prev_bank != NULL) { // exclude head bank with null prev
        bank->addr = prev_bank->addr + prev_bank->alloc_len; // starting addr + alloc len
        bank->prev = prev_bank;
        bank->next = NULL;
    }

    // alloc bank
    bank->is_free = 0;
    bank->alloc_len = len;
}

// request a bank with the size of the value being stored as len 
// ptr <- starting address(virtual); stored in R[28] <- 0 if can't allocate
void VM_malloc(uint32_t value) {
    uint32_t *ini_addr = NULL; // start addr to store in R[28]

    // cannot malloc size > virtual addr space
    if (value > HEAP_MEM) { 
            regs[R28] = 0;
            return;
    }
    // initialize head bank
    if (head_bank == NULL) {   
        struct heap_bank my_head_bank;
        head_bank = &my_head_bank;
        head_bank->is_free = 1;
        head_bank->addr = HEAP_START_ADDR;
        head_bank->prev = NULL; 
        head_bank->next = NULL;
    } 

    // B0 -> B2 -> B3 -> ... B127 
    // alloc free bank OR create new bank and alloc if null
    // if reach last bank B127 -> not able to allocate (0)
    struct heap_bank * current = head_bank;
    struct heap_bank * tmp_prev = NULL;
    while (value > BANK_SZ) { 
        if ( current_bank_sz() == NUM_BANK-1 || // at last bank (NULL)
                (current == tail_bank && tail_bank != NULL) ) { // at last bank (Not NULL)
            regs[R28] = 0;
            return;
        }

        if (current == NULL || current->is_free) { 
            alloc_bank(current, tmp_prev, BANK_SZ); // full alloc
            *ini_addr =  (ini_addr == NULL) ? current->addr : *ini_addr; // update start addr if needed
            value -= BANK_SZ;
        }
        tmp_prev = current;
        current = current->next;
    }

    // alloc the remaining value to next free bank
    // current bank free or null -> create & alloc
    // current bank not free, traverse until next free/null, unless reach last bank
    if (current == NULL || current->is_free) {
        alloc_bank(current, tmp_prev, value);
        *ini_addr =  (ini_addr == NULL) ? current->addr : *ini_addr; // update start addr if needed
    } else {
        current = current->next;
        while (current != NULL && !current->is_free) {
            if ( current == tail_bank ) { // reach last bank, and it is not free or null
                regs[R28] = 0;
                return;
            }
            tmp_prev = current;
            current = current->next;
        }
        alloc_bank(current, tmp_prev, value);
        *ini_addr =  (ini_addr == NULL) ? current->addr : *ini_addr; // update start addr if needed
    }
    regs[R28] = *ini_addr; // store start addr into R[28]
}

// free bank with specified STARTING addr as value being stored
// if addr was not allocated, an illegal operation error should be raised
void VM_free(uint32_t addr, uint32_t instruc) {
    struct heap_bank * current = head_bank; 
    while (current != NULL) {
        // free bank if addr matched and not already deallocated 
        if (current->addr == addr && !current->is_free) {
            current->is_free =  1;
            return;
        } 
        current = current->next;
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