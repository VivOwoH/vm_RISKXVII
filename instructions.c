#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "types.h"
#include "instructions.h"
#include "func_err.h"

uint32_t sign_extend(uint32_t n, int imm_bits) {
    // check MSB 1 or 0
    // MSB 1: sign extend left with 1
    // MSB 0: sign extend left with 0 (i.e. do nothing)
    if ( ((n >> (imm_bits - 1)) & 0x1) == 1) {
        n = n | (0xFFFFFFFF << imm_bits);
    }
    return n;
}

// ------------------------- DECODE --------------------------
void type_R(uint32_t instruc) {
    // func3 (14<-12 bits)
    // func7 (31<-25 bits)
    uint32_t func3 = (instruc >> 12) & 0x7;
    uint32_t func7 = instruc >> 25;

    uint32_t rd = (instruc >> 7) & 0x1F;
    uint32_t rs1 = (instruc >> 15) & 0x1F;
    uint32_t rs2 = (instruc >> 20) & 0x1F;

    if (func3 == 0x0) {
        switch (func7) {
        case 0x0:
            R_operation(ADD, rd, rs1, rs2);
            break;
        case 0x20:
            R_operation(SUB, rd, rs1, rs2);
            break;
        }
    }
    else if (func3 == 0x1 && func7 == 0x0) {
        R_operation(SLL, rd, rs1, rs2);
    }
    else if (func3 == 0x2 && func7 == 0x0) {
        R_operation(SLT, rd, rs1, rs2);
    }
    else if (func3 == 0x3 && func7 == 0x0) {
        R_operation(SLTU, rd, rs1, rs2);
    }
    else if (func3 == 0x4 && func7 == 0x0) {
        R_operation(XOR, rd, rs1, rs2);
    }
    else if (func3 == 0x5) {
        switch (func7) {
        case 0x0:
            R_operation(SRL, rd, rs1, rs2);
            break;
        case 0x20:
            R_operation(SRA, rd, rs1, rs2);
            break;
        }
    }
    else if (func3 == 0x6 && func7 == 0x0) {
        R_operation(OR, rd, rs1, rs2);
    }
    else if (func3 == 0x7 && func7 == 0x0) {
        R_operation(AND, rd, rs1, rs2);
    }
}

void type_I(uint32_t instruc, uint32_t opcode) {
    // func3 (14<-12 bits)
    uint32_t func3 = (instruc >> 12) & 0x7;

    uint32_t rd = (instruc >> 7) & 0x1F;
    uint32_t rs1 = (instruc >> 15) & 0x1F;
    uint32_t imm = sign_extend((instruc >> 20), 12); // 12 bits

    // // printf("func3 = %.2x\n", func3);

    if (func3 == 0x0) {
        switch (opcode) {
        case 0x13:
            I_operation(ADDI, rd, rs1, imm, instruc);
            break;
        case 0x3:
            I_operation(LB, rd, rs1, imm, instruc);
            break;
        case 0x67:
            I_operation(JALR, rd, rs1, imm, instruc);
            break;
        }
    }
    else if (func3 == 0x1) {
        I_operation(LH, rd, rs1, imm, instruc);
    }
    else if (func3 == 0x2) {
        switch (opcode) {
        case 0x13:
            I_operation(SLTI, rd, rs1, imm, instruc);
            break;
        case 0x3:
            I_operation(LW, rd, rs1, imm, instruc);
            break;
        }
    }
    else if (func3 == 0x3) {
        I_operation(SLTIU, rd, rs1, imm, instruc);
    }    
    else if (func3 == 0x4) {
        switch (opcode) {
        case 0x13:
            I_operation(XORI, rd, rs1, imm, instruc);
            break;
        case 0x3:
            I_operation(LBU, rd, rs1, imm, instruc);
            break;
        }
    }
    else if (func3 == 0x5) {
        I_operation(LHU, rd, rs1, imm, instruc);
    }    
    else if (func3 == 0x6) {
        I_operation(ORI, rd, rs1, imm, instruc);
    }
    else if (func3 == 0x7) {
        I_operation(ANDI, rd, rs1, imm, instruc);
    }

}

void type_S(uint32_t instruc) {
    // func3 (14<-12 bits)
    uint32_t func3 = (instruc >> 12) & 0x7;

    // // printf("%.2x | \n", func3);

    uint32_t rs1 = (instruc >> 15) & 0x1F;
    uint32_t rs2 = (instruc >> 20) & 0x1F;
    
    uint32_t imm = sign_extend( ( ((instruc >> 25) << 5 ) + ((instruc >> 7) & 0x1F) ), 12); // 12 bits
    
    if (func3 == 0x0)
        S_operation(SB, rs1, rs2, imm, instruc);
    else if (func3 == 0x1)
        S_operation(SH, rs1, rs2, imm, instruc);
    else if (func3 == 0x2)
        S_operation(SW, rs1, rs2, imm, instruc);
}

void type_SB(uint32_t instruc) {
    // func3 (14<-12 bits)
    uint32_t func3 = (instruc >> 12) & 0x7;

    // // printf("%.2x | \n", func3);

    uint32_t rs1 = (instruc >> 15) & 0x1F;
    uint32_t rs2 = (instruc >> 20) & 0x1F;

    uint32_t del1 = (instruc >> 7) & 0x1; // 11 -> 10
    uint32_t del2 = (instruc >> 8) & 0xF; // 4:1 -> 3:0
    uint32_t del3 = (instruc >> 25) & 0x3F; // 10:5 -> 9:4
    uint32_t del4 = (instruc >> 31); // 12 -> 11
    
    uint32_t imm = sign_extend( ((del4 << 11) + (del1 << 10) + (del3 << 4) + del2), 12); // 12 bits

    if (func3 == 0x0)
        SB_operation(BEQ, rs1, rs2, imm);
    else if (func3 == 0x1)
        SB_operation(BNE, rs1, rs2, imm);
    else if (func3 == 0x4)
        SB_operation(BLT, rs1, rs2, imm);
    else if (func3 == 0x5)
        SB_operation(BGE, rs1, rs2, imm);
    else if (func3 == 0x6)
        SB_operation(BLTU, rs1, rs2, imm);
    else if (func3 == 0x7)
        SB_operation(BGEU, rs1, rs2, imm);
}

void type_U(uint32_t instruc) {
    uint32_t rd = (instruc >> 7) & 0x1F;
    uint32_t imm = sign_extend( (instruc >> 12), 20); // 20 bits

    LUI(rd, imm);
}

void type_UJ(uint32_t instruc) {
    uint32_t rd = (instruc >> 7) & 0x1F;

    uint32_t del1 = (instruc >> 12) & 0xFF; // 19:12 -> 18:11
    uint32_t del2 = (instruc >> 20) & 0x1; // 11 -> 10
    uint32_t del3 = (instruc >> 21) & 0x3FF; // 10:1 -> 9:0
    uint32_t del4 = (instruc >> 31); // 20 -> 19
    
    uint32_t imm = sign_extend( (del4 << 19) + del3 + (del2 << 10) + (del1 << 11), 20); // 20 bits

    JAL(rd, imm);
}   

// ------------------------- MEMORY -----------------------------

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

// ------------------------- type R --------------------------------
void R_operation(int command, uint32_t rd, uint32_t rs1, uint32_t rs2) {
    switch (command)
    {
    case ADD:
        regs[rd] = regs[rs1] + regs[rs2];
        // printf("%d: add | rs1=%d, rs2=%d, rd=%d | regs[rd] = %d\n", regs[RPC], rs1, rs2, rd, regs[rd]);
        break;

    case SUB:
        regs[rd] = regs[rs1] - regs[rs2];
        // printf("%d: sub | regs[rd] = %d\n", regs[RPC], regs[rd]);
        break;
    
    case SLL: // logical left shift on the value in rs1 by the shift amount held in the lower 5 bits of rs2
        regs[rd] = regs[rs1] << regs[rs2];
        // printf("%d: sll | regs[rd] = %d\n", regs[RPC], regs[rd]);
        break;
    
    case SLT: // Place 1 in rd if rs1 < rs2 (both treated as signed numbers), else 0 is written to rd
        regs[rd] = ((int32_t) regs[rs1] < (int32_t) regs[rs2]) ? 1 : 0;
        // printf("%d: slt | regs[rd] = %d\n", regs[RPC], regs[rd]);
        break;
    
    case SLTU: // Place 1 in rd if rs1 < rs2 (both treated as unsigned numbers), else 0 is written to rd
        regs[rd] = (regs[rs1] < regs[rs2]) ? 1 : 0;
        // printf("%d: sltu | regs[rd] = %d\n", regs[RPC], regs[rd]);
        break;
    
    case XOR:
        regs[rd] = regs[rs1] ^ regs[rs2]; 
        // printf("%d: xor | regs[rd] = %d\n", regs[RPC], regs[rd]);
        break;
    
    case SRL: // shift right
        regs[rd] = regs[rs1] >> regs[rs2];
        // printf("%d: srl | regs[rd] = %d\n", regs[RPC], regs[rd]);
        break;
    
    case SRA: // rotate right - right most bit is moved to the left most after shifting.
        // original = after shifted
        regs[rd] =  regs[rs1] >> regs[rs2] |  // OR with original with 1 bit to right (i.e. leftmost vacant)
                    ( (regs[rs1] >> regs[rs2]) & 0x1 ) << (regs[rs2]+1); // get right most bit then extended til left most 
                                                                        // (i.e. amount held by rs2 plus 1)
        // printf("%d: sra | regs[rd] = %d\n", regs[RPC], regs[rd]);
        break;
    
    case OR:
        regs[rd] = regs[rs1] | regs[rs2];
        // printf("%d: or | regs[rd] = %d\n", regs[RPC], regs[rd]);
        break;
    
    case AND:
        regs[rd] = regs[rs1] & regs[rs2];
        // printf("%d: and | regs[rd] = %d\n", regs[RPC], regs[rd]);
        break;  
    
    default:
        break;
    }
}

// ------------------------- type I --------------------------------
void I_operation(int command, uint32_t rd, uint32_t rs1, uint32_t imm, uint32_t instruc) {
    switch (command)
    {
    case ADDI:
        regs[rd] = regs[rs1] + imm;
        // printf("%d: addi | rd=%d, rs1=%d, imm = %d | regs[rd] = %d \n", regs[RPC], rd, rs1, imm, regs[rd]);
        break;

    case JALR: // Jump to target code address from a register, and save the PC value of next instruction into a register.
        // printf("%d: jalr rd=%d, rs1=%d, regs[rs1]=%d, imm=%d ", regs[RPC], rd, rs1, regs[rs1], imm);
        regs[rd] = regs[RPC] + 4;
        regs[RPC] = regs[rs1] + imm - 4; // and no need to incremnent 4 to PC
        // printf("regs[RPC](x)=%d\n", regs[RPC]+4);
        break;

    // Load value from mem into a register
    // LB, LBU: 1 cell; LH, LHU: 2 cells; LW: 4 cells
    // LB(8), LH(16): sign extend
    case LB:
    case LBU:
    case LH:
    case LHU:
    case LW: {
        int num_cell = 1; // read how much mem
        num_cell = (command==LH || command==LHU) ? 2 : num_cell;
        num_cell = (command==LW) ? 4 : num_cell;
        uint32_t value = mem_read(regs[rs1] + imm, num_cell, instruc);
        // sign extend if required
        value = (command==LB) ? sign_extend(value, 8) : value;
        value = (command==LH) ? sign_extend(value, 16) : value;
        // load into register
        regs[rd] = value; 
        }
        break;

    case SLTI: // Set rd to 1 if the value in rs1 is smaller than imm, 0 otherwise
        regs[rd] = ((int32_t) regs[rs1] < imm) ? 1 : 0;
        // printf("%d: slti | regs[rd] = %d\n", regs[RPC], regs[rd]);
        break;

    case SLTIU: // Set rd to 1 if the value in rs1 is smaller than imm, 0 otherwise, unsigned
        regs[rd] = (regs[rs1] < imm) ? 1 : 0;
        // printf("%d: sltiu | regs[rd] = %d\n", regs[RPC], regs[rd]);
        break;

    case XORI:
        regs[rd] = regs[rs1] ^ imm;
        // printf("%d: xori | regs[rd] = %d\n", regs[RPC], regs[rd]);
        break;

    case ORI:
        regs[rd] = regs[rs1] | imm;
        // printf("%d: ori | regs[rd] = %d\n", regs[RPC], regs[rd]);
        break;

    case ANDI:
        regs[rd] = regs[rs1] & imm;
        // printf("%d: andi | regs[rd] = %d\n", regs[RPC], regs[rd]);
        break;
    
    default:
        break;
    }
}

// ------------------------- type S --------------------------------
void S_operation(int command, uint32_t rs1, uint32_t rs2, uint32_t imm, uint32_t instruc) {
    switch (command)
    {
    case SB: // store a 8-bit value to memory from a register
        // printf("%d: sb | rs1=%d, rs2=%d, imm=%d \n", regs[RPC], rs1, rs2, imm);
        mem_write((regs[rs1] + imm), regs[rs2] & 0xFF, 1, instruc);
        break;

    case SH: // store a 16-bit value to memory from a register
        // printf("%d: sh | rs1=%d, rs2=%d, imm=%d\n", regs[RPC], rs1, rs2, imm);
        mem_write((regs[rs1] + imm), regs[rs2] & 0xFFFF, 2, instruc);
        break;

    case SW: // store a 32-bit value to memory from a register
        // printf("%d: sw | rs1=%d, rs2=%d, imm=%d | addr=%.2x, value=%d\n", regs[RPC], rs1, rs2, imm, regs[rs1]+imm, regs[rs2]);
        mem_write((regs[rs1] + imm), regs[rs2], 4, instruc);
        break;

    default:
        break;
    }
}

// ------------------------- type SB --------------------------------
void SB_operation(int command, uint32_t rs1, uint32_t rs2, uint32_t imm) {
    switch (command)
    {
    case BEQ: // branch if equal
        // printf("%d: beq | ", regs[RPC]);
        if (regs[rs1] == regs[rs2]) {
            regs[RPC] = (regs[RPC] - 4) + (imm << 1);
        }
        // printf("imm=%d, regs[rs1]=%d, regs[rs2]=%d | regs[RPC](x)=%d\n", imm, regs[rs1], regs[rs2], regs[RPC]+4);
        break;

    case BNE: // branch if not equal
            // printf("%d: bne | ", regs[RPC]);
        if (regs[rs1] != regs[rs2]) {
            regs[RPC] = (regs[RPC] - 4) + (imm << 1);
        }
        // printf("rs1=%d, rs2=%d, imm=%d | regs[RPC](x)=%d\n", rs1, rs2, imm, regs[RPC]+4);
        break;

    case BLT: // branch if less than, signed
        // printf("%d: blt | ", regs[RPC]);
        if ((int32_t) regs[rs1] < (int32_t) regs[rs2]) {
            regs[RPC] = (regs[RPC] - 4) + (imm << 1);
        }
        // printf("rs1=%d, rs2=%d, imm=%d | regs[RPC](x)=%d\n", rs1, rs2, imm, regs[RPC]+4);
        break;

    case BLTU: // branch if less than, unsigned
            // printf("%d: bltu | ", regs[RPC]);
        if (regs[rs1] < regs[rs2]) {
            regs[RPC] = (regs[RPC] - 4) + (imm << 1);
        }
        // printf("rs1=%d, rs2=%d, imm=%d | regs[RPC](x)=%d\n", rs1, rs2, imm, regs[RPC]+4);
        break;

    case BGE: // branch if Greater Than or Equal
        // printf("%d: bge | ", regs[RPC]);
        if ((int32_t) regs[rs1] >= (int32_t) regs[rs2]) {
            regs[RPC] = (regs[RPC] - 4) + (imm << 1);
        }
        // printf("rs1=%d, rs2=%d, imm=%d | regs[RPC](x)=%d\n", rs1, rs2, imm, regs[RPC]+4);
        break;

    case BGEU: // branch if Greater Than or Equal, unsigned
        // printf("%d: bgeu | ", regs[RPC]);
        if (regs[rs1] >= regs[rs2]) {
            regs[RPC] = (regs[RPC] - 4) + (imm << 1);
        }
        // printf("rs1=%d, rs2=%d, imm=%d | regs[RPC](x)=%d\n", rs1, rs2, imm, regs[RPC]+4);
        break;

    default:
        break;
    }
}

// ------------------------- type U --------------------------------
void LUI(uint32_t rd, uint32_t imm) {
    regs[rd] = imm << 12; // sign extend to 32 bits (upper from reg, lower to 0)
    // printf("%d: lui | rd=%d, imm=%.2x, regs[rd] = %d\n", regs[RPC], rd, imm, regs[rd]);
}

// ------------------------- type UJ --------------------------------
void JAL(uint32_t rd, uint32_t imm) { 
    // printf("%d: jal | rd=%d, ", regs[RPC], rd);
    regs[rd] = regs[RPC] + 4; // save PC value of next instruc
    regs[RPC] = (regs[RPC] - 4) + (imm << 1); // jump to target (and no need to incremnent 4 to PC)
    // printf("regs[rd]=%d, imm = %d, regs[RPC](x) = %d\n", regs[rd], imm, regs[RPC]+4);
}