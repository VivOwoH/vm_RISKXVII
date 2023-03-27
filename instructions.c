#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "types.h"
#include "instructions.h"
#include "vr.h"

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
            ADD(rd, rs1, rs2);
            break;
        case 0x20:
            SUB(rd, rs1, rs2);
            break;
        }
    }
    else if (func3 == 0x1 && func7 == 0x0) {
        SLL(rd, rs1, rs2);
    }
    else if (func3 == 0x2 && func7 == 0x0) {
        SLT(rd, rs1, rs2);
    }
    else if (func3 == 0x3 && func7 == 0x0) {
        SLTU(rd, rs1, rs2);
    }
    else if (func3 == 0x4 && func7 == 0x0) {
        XOR(rd, rs1, rs2);
    }
    else if (func3 == 0x5) {
        switch (func7) {
        case 0x0:
            SRL(rd, rs1, rs2);
            break;
        case 0x20:
            SRA(rd, rs1, rs2);
            break;
        }
    }
    else if (func3 == 0x6 && func7 == 0x0) {
        OR(rd, rs1, rs2);
    }
    else if (func3 == 0x7 && func7 == 0x0) {
        AND(rd, rs1, rs2);
    }
}

void type_I(uint32_t instruc, uint32_t opcode) {
    // func3 (14<-12 bits)
    uint32_t func3 = (instruc >> 12) & 0x7;

    uint32_t rd = (instruc >> 7) & 0x1F;
    uint32_t rs1 = (instruc >> 15) & 0x1F;
    uint32_t imm = sign_extend((instruc >> 20), 12); // 12 bits

    if (func3 == 0x0) {
        switch (opcode) {
        case 0x13:
            ADDI(rd, rs1, imm);
            break;
        case 0x3:
            LB(rd, rs1, imm, instruc);
            break;
        case 0x67:
            JALR(rd, rs1, imm);
            break;
        }
    }
    else if (func3 == 0x1) {
        LH(rd, rs1, imm, instruc);
    }
    else if (func3 == 0x2) {
        switch (opcode) {
        case 0x13:
            SLTI(rd, rs1, imm);
            break;
        case 0x3:
            LW(rd, rs1, imm, instruc);
            break;
        }
    }
    else if (func3 == 0x3) {
        SLTIU(rd, rs1, imm);
    }    
    else if (func3 == 0x4) {
        switch (opcode) {
        case 0x13:
            XORI(rd, rs1, imm);
            break;
        case 0x3:
            LBU(rd, rs1, imm, instruc);
            break;
        }
    }
    else if (func3 == 0x5) {
        LHU(rd, rs1, imm, instruc);
    }    
    else if (func3 == 0x6) {
        ORI(rd, rs1, imm);
    }
    else if (func3 == 0x7) {
        ANDI(rd, rs1, imm);
    }

}

void type_S(uint32_t instruc) {
    // func3 (14<-12 bits)
    uint32_t func3 = (instruc >> 12) & 0x7;

    uint32_t rs1 = (instruc >> 15) & 0x1F;
    uint32_t rs2 = (instruc >> 20) & 0x1F;
    
    uint32_t imm = sign_extend( ( ((instruc >> 25) << 5 ) + ((instruc >> 7) & 0x1F) ), 12); // 12 bits
    
    if (func3 == 0x0)
        SB(rs1, rs2, imm, instruc);
    else if (func3 == 0x1)
        SH(rs1, rs2, imm, instruc);
    else if (func3 == 0x2)
        SW(rs1, rs2, imm, instruc);
}

void type_SB(uint32_t instruc) {
    // func3 (14<-12 bits)
    uint32_t func3 = (instruc >> 12) & 0x7;

    uint32_t rs1 = (instruc >> 15) & 0x1F;
    uint32_t rs2 = (instruc >> 20) & 0x1F;

    uint32_t del1 = (instruc >> 7) & 0x1; // 11 -> 10
    uint32_t del2 = (instruc >> 8) & 0xF; // 4:1 -> 3:0
    uint32_t del3 = (instruc >> 25) & 0x3F; // 10:5 -> 9:4
    uint32_t del4 = (instruc >> 31); // 12 -> 11
    
    uint32_t imm = sign_extend( ((del4 << 11) + (del1 << 10) + (del3 << 4) + del2), 12); // 12 bits

    if (func3 == 0x0)
        BEQ(rs1, rs2, imm);
    else if (func3 == 0x1)
        BNE(rs1, rs2, imm);
    else if (func3 == 0x4)
        BLT(rs1, rs2, imm);
    else if (func3 == 0x5)
        BGE(rs1, rs2, imm);
    else if (func3 == 0x6)
        BLTU(rs1, rs2, imm);
    else if (func3 == 0x7)
        BGEU(rs1, rs2, imm);
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
            // read from instruc or data allowed
            if (addr < (INSTRUC_MEM + DATA_MEM)) {
                uint32_t value = 0;
                for (int i = 0; i < num_cell; i++) {
                    value += memptr->inst_mem[addr+i] << (8 * (num_cell-1-i)); // e.g. 32bits:8*3, 8*2, 8*1, 8*0
                }
                return value;
            } else if (1) {
                // TODO: heap bank addr range
            } else {
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
        default:
            // only write to data allowed
            if (addr >= INSTRUC_MEM && addr < (INSTRUC_MEM + DATA_MEM)) {
                for (int i = 0; i < num_cell; i++) {
                    memptr->inst_mem[addr+i] = ( value >> (8 * (num_cell-1-i)) ) & 0xFF; 
                }
            } else {
                err_illegal_op(instruc);
            }            
            break;
    }
    return 0;
}

// ------------------------- EXECUTE --------------------------------
void ADD(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    memptr->R[rd] = memptr->R[rs1] + memptr->R[rs2];
    // printf("%d: add | rs1=%d, rs2=%d, rd=%d | memptr->R[rd] = %d\n", memptr->R[RPC], rs1, rs2, rd, memptr->R[rd]);
}

void SUB(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    memptr->R[rd] = memptr->R[rs1] - memptr->R[rs2];
    // printf("%d: sub | memptr->R[rd] = %d\n", memptr->R[RPC], memptr->R[rd]);
}

// logical left shift on the value in rs1 by the shift amount held in the lower 5 bits of rs2
void SLL(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    memptr->R[rd] = memptr->R[rs1] << memptr->R[rs2];
    // printf("%d: ssl | memptr->R[rd] = %d\n", memptr->R[RPC], memptr->R[rd]);
}

// Place 1 in rd if rs1 < rs2 (both treated as signed numbers), else 0 is written to rd
void SLT(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    memptr->R[rd] = ((int32_t) memptr->R[rs1] < (int32_t) memptr->R[rs2]) ? 1 : 0;
    // printf("%d: slt | memptr->R[rd] = %d\n", memptr->R[RPC], memptr->R[rd]);
}

// Place 1 in rd if rs1 < rs2 (both treated as unsigned numbers), else 0 is written to rd
void SLTU(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    memptr->R[rd] = (memptr->R[rs1] < memptr->R[rs2]) ? 1 : 0;
    // printf("%d: sltu | memptr->R[rd] = %d\n", memptr->R[RPC], memptr->R[rd]);
}

void XOR(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    memptr->R[rd] = memptr->R[rs1] ^ memptr->R[rs2]; 
    // printf("%d: xor | memptr->R[rd] = %d\n", memptr->R[RPC], memptr->R[rd]);
}

// shift right
void SRL(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    memptr->R[rd] = memptr->R[rs1] >> memptr->R[rs2];
    // printf("%d: srl | memptr->R[rd] = %d\n", memptr->R[RPC], memptr->R[rd]);
}

// rotate right - right most bit is moved to the left most after shifting.
void SRA(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    // original = after shifted
    memptr->R[rd] =  memptr->R[rs1] >> memptr->R[rs2] |  // OR with original with 1 bit to right (i.e. leftmost vacant)
                ( (memptr->R[rs1] >> memptr->R[rs2]) & 0x1 ) << (memptr->R[rs2]+1); // get right most bit then extended til left most 
    // printf("%d: sra | memptr->R[rd] = %d\n", memptr->R[RPC], memptr->R[rd]);
}

void OR(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    memptr->R[rd] = memptr->R[rs1] | memptr->R[rs2];
    // printf("%d: or | memptr->R[rd] = %d\n", memptr->R[RPC], memptr->R[rd]);
}

void AND(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    memptr->R[rd] = memptr->R[rs1] & memptr->R[rs2];
    // printf("%d: and | memptr->R[rd] = %d\n", memptr->R[RPC], memptr->R[rd]);
}

void ADDI(uint32_t rd, uint32_t rs1, uint32_t imm) {
    memptr->R[rd] = memptr->R[rs1] + imm;
    // printf("%d: addi | rd=%d, rs1=%d, imm = %d | memptr->R[rd] = %d \n", memptr->R[RPC], rd, rs1, imm, memptr->R[rd]);
}

// Load a 8-bit value from memory into a register, and sign extend the value
void LB(uint32_t rd, uint32_t rs1, uint32_t imm, uint32_t instruc) {
    // i.e. LSB 8 bits of value returned from reading mem
    // printf("%d: lb | rs1=%d, imm=%d | memptr->R[rd] = %d\n", memptr->R[RPC], rs1, imm, memptr->R[rd]);
    memptr->R[rd] = sign_extend(mem_read(memptr->R[rs1] + imm, 1, instruc), 8);
}

// Load a 16-bit value from memory into a register, and sign extend the value.
void LH(uint32_t rd, uint32_t rs1, uint32_t imm, uint32_t instruc) {
    // i.e. LSB 16 bits of value returned from reading mem
    // printf("%d: lh | memptr->R[rd] = %d\n", memptr->R[RPC], memptr->R[rd]);
    memptr->R[rd] = sign_extend(mem_read(memptr->R[rs1] + imm, 2, instruc), 16);
}

// Load a 32-bit value from memory into a register
void LW(uint32_t rd, uint32_t rs1, uint32_t imm, uint32_t instruc) {
    // printf("%d: lw | rd=%d, rs1=%d, imm = %d | addr=%.2x\n", memptr->R[RPC], rd, rs1, imm, memptr->R[rs1]+imm);
    memptr->R[rd] = mem_read(memptr->R[rs1] + imm, 4, instruc);
    // printf("result: memptr->R[rd]=%d\n", memptr->R[rd]);
}

// Load a 8-bit value from memory into a register
void LBU(uint32_t rd, uint32_t rs1, uint32_t imm, uint32_t instruc) {
    // printf("%d: lbu | rd=%d, rs1=%d, imm=%d, addr=%d\n", memptr->R[RPC], rd, rs1, imm, memptr->R[rs1]+imm);
    memptr->R[rd] = mem_read(memptr->R[rs1] + imm, 1, instruc);
    // printf("result memptr->R[rd]=%d\n", memptr->R[rd]);
}

// Load a 16-bit value from memory into a register
void LHU(uint32_t rd, uint32_t rs1, uint32_t imm, uint32_t instruc) {
    // printf("%d: lhu | memptr->R[rd] = %d\n", memptr->R[RPC], memptr->R[rd]);
    memptr->R[rd] = mem_read(memptr->R[rs1] + imm, 2, instruc);
}

// Set rd to 1 if the value in rs1 is smaller than imm, 0 otherwise
void SLTI(uint32_t rd, uint32_t rs1, uint32_t imm) {
    memptr->R[rd] = ((int32_t) memptr->R[rs1] < imm) ? 1 : 0;
    // printf("%d: slti | memptr->R[rd] = %d\n", memptr->R[RPC], memptr->R[rd]);
}

// Set rd to 1 if the value in rs1 is smaller than imm, 0 otherwise, unsigned
void SLTIU(uint32_t rd, uint32_t rs1, uint32_t imm) {
    memptr->R[rd] = (memptr->R[rs1] < imm) ? 1 : 0;
    // printf("%d: sltiu | memptr->R[rd] = %d\n", memptr->R[RPC], memptr->R[rd]);
}

void XORI(uint32_t rd, uint32_t rs1, uint32_t imm) {
    memptr->R[rd] = memptr->R[rs1] ^ imm;
    // printf("%d: xori | memptr->R[rd] = %d\n", memptr->R[RPC], memptr->R[rd]);
}

void ORI(uint32_t rd, uint32_t rs1, uint32_t imm) {
    memptr->R[rd] = memptr->R[rs1] | imm;
    // printf("%d: ori | memptr->R[rd] = %d\n", memptr->R[RPC], memptr->R[rd]);
}

void ANDI(uint32_t rd, uint32_t rs1, uint32_t imm) {
    memptr->R[rd] = memptr->R[rs1] & imm;
    // printf("%d: andi | memptr->R[rd] = %d\n", memptr->R[RPC], memptr->R[rd]);
}

// store a 8-bit value to memory from a register
void SB(uint32_t rs1, uint32_t rs2, uint32_t imm, uint32_t instruc) {
    // printf("%d: sb | rs1=%d, rs2=%d, imm=%d \n", memptr->R[RPC], rs1, rs2, imm);
    mem_write((memptr->R[rs1] + imm), memptr->R[rs2] & 0xFF, 1, instruc);
}

// store a 16-bit value to memory from a register
void SH(uint32_t rs1, uint32_t rs2, uint32_t imm, uint32_t instruc) {
    // printf("%d: sh | rs1=%d, rs2=%d, imm=%d\n", memptr->R[RPC], rs1, rs2, imm);
    mem_write((memptr->R[rs1] + imm), memptr->R[rs2] & 0xFFFF, 2, instruc);
}

// store a 32-bit value to memory from a register
void SW(uint32_t rs1, uint32_t rs2, uint32_t imm, uint32_t instruc) {
    // printf("%d: sw | rs1=%d, rs2=%d, imm=%d | addr=%.2x, value=%d\n", memptr->R[RPC], rs1, rs2, imm, memptr->R[rs1]+imm, memptr->R[rs2]);
    mem_write((memptr->R[rs1] + imm), memptr->R[rs2], 4, instruc);
}

// branch if equal
void BEQ(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    // printf("%d: beq | ", memptr->R[RPC]);
    if (memptr->R[rs1] == memptr->R[rs2]) {
        memptr->R[RPC] = (memptr->R[RPC] - 4) + (imm << 1);
    }
    // printf("imm=%d, memptr->R[rs1]=%d, memptr->R[rs2]=%d | memptr->R[RPC](x)=%d\n", imm, memptr->R[rs1], memptr->R[rs2], memptr->R[RPC]+4);
}

// branch if not equal
void BNE(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    // printf("%d: bne | ", memptr->R[RPC]);
    if (memptr->R[rs1] != memptr->R[rs2]) {
        memptr->R[RPC] = (memptr->R[RPC] - 4) + (imm << 1);
    }
    // printf("rs1=%d, rs2=%d, imm=%d | memptr->R[RPC](x)=%d\n", rs1, rs2, imm, memptr->R[RPC]+4);
}

// branch if less than, signed
void BLT(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    // printf("%d: blt | ", memptr->R[RPC]);
    if ((int32_t) memptr->R[rs1] < (int32_t) memptr->R[rs2]) {
        memptr->R[RPC] = (memptr->R[RPC] - 4) + (imm << 1);
    }
    // printf("rs1=%d, rs2=%d, imm=%d | memptr->R[RPC](x)=%d\n", rs1, rs2, imm, memptr->R[RPC]+4);
}

// branch if less than, unsigned
void BLTU(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    // printf("%d: bltu | ", memptr->R[RPC]);
    if (memptr->R[rs1] < memptr->R[rs2]) {
        memptr->R[RPC] = (memptr->R[RPC] - 4) + (imm << 1);
    }
    // printf("rs1=%d, rs2=%d, imm=%d | memptr->R[RPC](x)=%d\n", rs1, rs2, imm, memptr->R[RPC]+4);
}

// branch if Greater Than or Equal
void BGE(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    // printf("%d: bge | ", memptr->R[RPC]);
    if ((int32_t) memptr->R[rs1] >= (int32_t) memptr->R[rs2]) {
        memptr->R[RPC] = (memptr->R[RPC] - 4) + (imm << 1);
    }
    // printf("rs1=%d, rs2=%d, imm=%d | memptr->R[RPC](x)=%d\n", rs1, rs2, imm, memptr->R[RPC]+4);
}

// branch if Greater Than or Equal, unsigned
void BGEU(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    // printf("%d: bgeu | ", memptr->R[RPC]);
    if (memptr->R[rs1] >= memptr->R[rs2]) {
        memptr->R[RPC] = (memptr->R[RPC] - 4) + (imm << 1);
    }
    // printf("rs1=%d, rs2=%d, imm=%d | memptr->R[RPC](x)=%d\n", rs1, rs2, imm, memptr->R[RPC]+4);
}   

void LUI(uint32_t rd, uint32_t imm) {
    memptr->R[rd] = imm << 12; // sign extend to 32 bits (upper from reg, lower to 0)
    // printf("%d: lui | rd=%d, imm=%.2x, memptr->R[rd] = %d\n", memptr->R[RPC], rd, imm, memptr->R[rd]);
}

void JAL(uint32_t rd, uint32_t imm) { 
    // printf("%d: jal | rd=%d, ", memptr->R[RPC], rd);
    memptr->R[rd] = memptr->R[RPC] + 4; // save PC value of next instruc
    memptr->R[RPC] = (memptr->R[RPC] - 4) + (imm << 1); // jump to target (and no need to incremnent 4 to PC)
    // printf("memptr->R[rd]=%d, imm = %d, memptr->R[RPC](x) = %d\n", memptr->R[rd], imm, memptr->R[RPC]+4);
}

// Jump to target code address from a register, and save the PC value of next instruction into a register.
void JALR(uint32_t rd, uint32_t rs1, uint32_t imm) {
    // printf("%d: jalr rd=%d, memptr->R[rs1]=%d, imm=%d ", memptr->R[RPC], rd, memptr->R[rs1], imm);
    memptr->R[rd] = memptr->R[RPC] + 4;
    memptr->R[RPC] = memptr->R[rs1] + imm - 4; // and no need to incremnent 4 to PC
    // printf("memptr->R[RPC](x)=%d\n", memptr->R[RPC]+4);
}