#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "vr.h"

// print value stored as a single ASCII encoded character to stdout
void Console_Write_char(uint32_t value) {
    fprintf(stdout, "%c\n", value);
}

// print the value being stored as a single 32-bit signed int in decimal format to stdout
void Console_Write_int(uint32_t value) {
    fprintf(stdout, "%d\n", value);
}

// print the value being stored as a single 32-bit (4bytes) unsigned int in lower case hexadecimal format to stdout
void Console_Write_uint(uint32_t value) {
    fprintf(stdout, "%.6x\n", value);
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

void Dump_PC() {

}

void Dump_reg_bank() {

}

void Dump_mem_word() {
    
}