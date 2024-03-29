#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "types.h"
#include "instructions.h"
#include "func_err.h"

// MEMORY
struct mem mymem;
struct mem * memptr = &mymem;

// REGISTERS
uint32_t regs[NUM_REG] = {0};

// HEAP
struct heap_bank * heap[NUM_BANK] = {NULL};

// FORWARD DECLARATIONS
int map_image(FILE *, struct mem *);
void fetch_instruc(struct mem *);


int main(int argc, char **argv) {

    if (argc != 2) {
        puts("[output] [memory image]");
        exit(1);
    }

    FILE * fp;
    fp = fopen(argv[1], "rb");

    if (fp  == NULL) {
		puts("Unable to open file");
		exit(2);
	}

    // initialize heap
    for (int i = 0; i < NUM_BANK; i++) {
        heap[i] = &memptr->heap_mem[i]; // ptr->block
        heap[i]->is_free = 1;
        heap[i]->addr = HEAP_START_ADDR + (BANK_SZ * i);
        heap[i]->prev = NULL; // link for a malloc
        heap[i]->next = NULL;
    }

    map_image(fp, memptr);
    fetch_instruc(memptr);
    return 0;
}

int map_image(FILE * fp, struct mem * memptr) {

    int size; // given file should be 2048 bytes
    
    fseek(fp, 0, SEEK_END); // eof
    size = ftell(fp); // get current fp
    fseek(fp, 0, SEEK_SET); // rewind

    if (size > sizeof(memptr->inst_data_mem)) {
        puts("File larger than expected.");
        exit(6);
    }

    // sizeof each mem = 1 byte
    return fread(memptr->inst_data_mem, sizeof(memptr->inst_data_mem[0]), 
                    sizeof(memptr->inst_data_mem)/sizeof(memptr->inst_data_mem[0]), fp);
}

void fetch_instruc(struct mem * memptr) {
    
    while (regs[RPC] < INSTRUC_MEM) {
        // read 4 uint8 to get uint32 instruction
        // LSB to MSB
        uint8_t token1 = memptr->inst_data_mem[regs[RPC]++];
        uint8_t token2 = memptr->inst_data_mem[regs[RPC]++];
        uint8_t token3 = memptr->inst_data_mem[regs[RPC]++];
        uint8_t token4 = memptr->inst_data_mem[regs[RPC]++]; // accessing parts of instruc
        regs[RPC] -= 4; // reset to start of instruc
        uint32_t instruc = (token4 << 24) + (token3 << 16) + (token2 << 8) + token1;
        
        // OPCODE (right most 7 bits)
        uint32_t opcode = (instruc << 25) >> 25;

        // some common sections
        uint32_t rs1 = (instruc >> 15) & 0x1F; // R,I,S,SB
        uint32_t rs2 = (instruc >> 20) & 0x1F; // R,S,SB
        uint32_t func3 = (instruc >> 12) & 0x7; // R,I,S,SB
        uint32_t rd = (instruc >> 7) & 0x1F; // R,I,U,UJ
        
        /* Summary:
        - type R: 0110011 = 33
        - type I: 0010011 = 13 / 0000011 = 3/ 1100111 = 67
        - type S: 0100011 = 23
        - type SB: 1100011 = 63
        - type U: 0110111 = 37 [lui]
        - type UJ: 1101111 = 6F [jal] */
        
        switch (opcode)
        {
            case 0x33:
                type_R(instruc, rs1, rs2, func3, rd);
                break;
                
            case 0x13:
            case 0x3:
            case 0x67:
                type_I(instruc, rs1, func3, rd, opcode);
                break;
          
            case 0x23:
                type_S(instruc, rs1, rs2, func3);
                break;
                
            case 0x63:
                type_SB(instruc, rs1, rs2, func3);
                break;
                
            case 0x37:
                type_U(instruc, rd);
                break;
                
            case 0x6F:
                type_UJ(instruc, rd);
                break;
                
            default:
                err_not_implemented(instruc);
        }
        regs[R0] = 0; // clear register 0
        regs[RPC] += 4; // increment PC 
    }
}

 

