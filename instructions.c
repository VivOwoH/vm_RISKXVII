#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "types.h"
#include "instructions.h"
#include "vr.h"

uint32_t sign_extend(uint32_t n, int imm_bits) {
    // check MSB 1 or 0
    // MSB 1: sign extend left with 1
    // MSB 0: sign extend left with 0 (i.e. do nothing)
    if ( ((n >> (imm_bits - 1)) & 0x1) == 1) {
        n = n | (0xFFFFFFFF << imm_bits); // flip all bits with OR to store the neg number
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

    // printf("func3 = %.2x\n", func3);

    if (func3 == 0x0) {
        switch (opcode) {
        case 0x13:
            ADDI(rd, rs1, imm);
            break;
        case 0x3:
            LB(rd, rs1, imm);
            break;
        case 0x67:
            JALR(rd, rs1, imm);
            break;
        }
    }
    else if (func3 == 0x1) {
        LH(rd, rs1, imm);
    }
    else if (func3 == 0x2) {
        switch (opcode) {
        case 0x13:
            SLTI(rd, rs1, imm);
            break;
        case 0x3:
            LW(rd, rs1, imm);
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
            LBU(rd, rs1, imm);
            break;
        }
    }
    else if (func3 == 0x5) {
        LHU(rd, rs1, imm);
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

    // printf("%.2x | \n", func3);

    uint32_t rs1 = (instruc >> 15) & 0x1F;
    uint32_t rs2 = (instruc >> 20) & 0x1F;
    
    uint32_t imm = sign_extend( ( ((instruc >> 25) << 5 ) + ((instruc >> 7) & 0x1F) ), 12); // 12 bits
    
    if (func3 == 0x0)
        SB(rs1, rs2, imm);
    else if (func3 == 0x1)
        SH(rs1, rs2, imm);
    else if (func3 == 0x2)
        SW(rs1, rs2, imm);
}

void type_SB(uint32_t instruc) {
    // func3 (14<-12 bits)
    uint32_t func3 = (instruc >> 12) & 0x7;

    // printf("%.2x | \n", func3);

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
uint32_t mem_read(uint32_t addr) {
    switch (addr) {
        case (C_Read_char):
            puts("-------console read char-------"); 
            return Console_Read_char();
        case (C_Read_int):
            puts("---------console read int-------"); 
            return Console_Read_int();
    }
    return 0;
}

uint32_t mem_write(uint32_t addr, uint32_t value) {
    switch (addr) {
        case (C_Write_char):
            puts("---------console write char-------"); 
            Console_Write_char(value);
            break;
        case (C_Write_int):
            puts("---------console write int-------"); 
            Console_Write_int(value);
            break;
        case (C_Write_uint):
            puts("---------console write uint-------"); 
            Console_Write_uint(value);
            break;
        case (Halt):
            Console_Halt();
            break;
        case (D_PC):
            puts("---------dump pc-------\n"); 
            Dump_PC();
            break;
        case (D_reg_bank):
            puts("---------dump reg bank-------\n"); 
            Dump_reg_bank();
            break;
        case (D_mem_word):
            puts("---------dump mem word-------\n"); 
            Dump_mem_word();
            break;
    }
    return 0;
}

// ------------------------- EXECUTE --------------------------------
void ADD(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    regs[rd] = regs[rs1] + regs[rs2];
    printf("add | rs1=%d, rs2=%d | regs[rd] = %d\n", rs1, rs2, regs[rd]);
}

void SUB(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    regs[rd] = regs[rs1] - regs[rs2];
    printf("sub | regs[rd] = %d\n", regs[rd]);
}

// logical left shift on the value in rs1 by the shift amount held in the lower 5 bits of rs2
void SLL(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    regs[rd] = regs[rs1] << regs[rs2];
    printf("ssl | regs[rd] = %d\n", regs[rd]);
}

// Place 1 in rd if rs1 < rs2 (both treated as signed numbers), else 0 is written to rd
void SLT(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    regs[rd] = ((int32_t) regs[rs1] < (int32_t) regs[rs2]) ? 1 : 0;
    printf("slt | regs[rd] = %d\n", regs[rd]);
}

// Place 1 in rd if rs1 < rs2 (both treated as unsigned numbers), else 0 is written to rd
void SLTU(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    regs[rd] = (regs[rs1] < regs[rs2]) ? 1 : 0;
    printf("sltu | regs[rd] = %d\n", regs[rd]);
}

void XOR(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    regs[rd] = regs[rs1] ^ regs[rs2]; 
    printf("xor | regs[rd] = %d\n", regs[rd]);
}

// shift right
void SRL(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    regs[rd] = regs[rs1] >> regs[rs2];
    printf("srl | regs[rd] = %d\n", regs[rd]);
}

// rotate right - right most bit is moved to the left most after shifting.
void SRA(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    // original = after shifted
    regs[rd] =  regs[rs1] >> regs[rs2] |  // OR with original with 1 bit to right (i.e. leftmost vacant)
                ( (regs[rs1] >> regs[rs2]) & 0x1 ) << (regs[rs2]+1); // get right most bit then extended til left most 
    printf("sra | regs[rd] = %d\n", regs[rd]);
}

void OR(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    regs[rd] = regs[rs1] | regs[rs2];
    printf("or | regs[rd] = %d\n", regs[rd]);
}

void AND(uint32_t rd, uint32_t rs1, uint32_t rs2) {
    regs[rd] = regs[rs1] & regs[rs2];
    printf("and | regs[rd] = %d\n", regs[rd]);
}

void ADDI(uint32_t rd, uint32_t rs1, uint32_t imm) {
    regs[rd] = regs[rs1] + imm;
    printf("addi | rd=%d, rs1=%d, imm = %d | regs[rd] = %d \n", rd, rs1, imm, regs[rd]);
}

// Load a 8-bit value from memory into a register, and sign extend the value
void LB(uint32_t rd, uint32_t rs1, uint32_t imm) {
    // i.e. LSB 8 bits of value returned from mem_read
    printf("lb | rs1=%d, imm=%d | regs[rd] = %d\n", rs1, imm, regs[rd]);
    regs[rd] = sign_extend(mem_read(regs[rs1] + imm) & 0xFF, 8);
}

// Load a 16-bit value from memory into a register, and sign extend the value.
void LH(uint32_t rd, uint32_t rs1, uint32_t imm) {
    // i.e. LSB 16 bits of value returned from mem_read
    printf("lh | regs[rd] = %d\n", regs[rd]);
    regs[rd] = sign_extend(mem_read(regs[rs1] + imm) & 0xFFFF, 16);
}

// Load a 32-bit value from memory into a register
void LW(uint32_t rd, uint32_t rs1, uint32_t imm) {
    printf("lw | rd=%d, rs1=%d, imm = %d | regs[rs1]+imm=%d, ", rd, rs1, imm, regs[rs1]+imm);
    regs[rd] = mem_read(regs[rs1] + imm);
    printf("result: regs[rd]=%d\n", regs[rd]);
}

// Set rd to 1 if the value in rs1 is smaller than imm, 0 otherwise
void SLTI(uint32_t rd, uint32_t rs1, uint32_t imm) {
    regs[rd] = ((int32_t) regs[rs1] < imm) ? 1 : 0;
    printf("slti | regs[rd] = %d\n", regs[rd]);
}

// Set rd to 1 if the value in rs1 is smaller than imm, 0 otherwise, unsigned
void SLTIU(uint32_t rd, uint32_t rs1, uint32_t imm) {
    regs[rd] = (regs[rs1] < imm) ? 1 : 0;
    printf("sltiu | regs[rd] = %d\n", regs[rd]);
}

void XORI(uint32_t rd, uint32_t rs1, uint32_t imm) {
    regs[rd] = regs[rs1] ^ imm;
    printf("xori | regs[rd] = %d\n", regs[rd]);
}

// Load a 8-bit value from memory into a register
void LBU(uint32_t rd, uint32_t rs1, uint32_t imm) {
    printf("lbu | rd=%d, rs1=%d, imm=%d, ", rd, rs1, imm);
    regs[rd] = mem_read(regs[rs1] + imm) & 0xFF;
    printf("result regs[rd]=%d\n", regs[rd]);
}

// Load a 16-bit value from memory into a register
void LHU(uint32_t rd, uint32_t rs1, uint32_t imm) {
    printf("lhu | regs[rd] = %d\n", regs[rd]);
    regs[rd] = mem_read(regs[rs1] + imm) & 0xFFFF;
}

void ORI(uint32_t rd, uint32_t rs1, uint32_t imm) {
    regs[rd] = regs[rs1] | imm;
    printf("ori | regs[rd] = %d\n", regs[rd]);
}

void ANDI(uint32_t rd, uint32_t rs1, uint32_t imm) {
    regs[rd] = regs[rs1] & imm;
    printf("andi | regs[rd] = %d\n", regs[rd]);
}

// store a 8-bit value to memory from a register
void SB(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    printf("sb | rs1=%d, rs2=%d, imm=%d \n", rs1, rs2, imm);
    mem_write((regs[rs1] + imm), regs[rs2] & 0xFF);
}

// store a 16-bit value to memory from a register
void SH(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    printf("sh | rs1=%d, rs2=%d, imm=%d\n", rs1, rs2, imm);
    mem_write((regs[rs1] + imm), regs[rs2] & 0xFFFF);
}

// store a 32-bit value to memory from a register
void SW(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    printf("sw | rs1=%d, rs2=%d, imm=%d\n", rs1, rs2, imm);
    mem_write((regs[rs1] + imm), regs[rs2]);
}

// branch if equal
void BEQ(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if (regs[rs1] == regs[rs2]) {
        regs[RPC] = (regs[RPC] - 4) + (imm << 1);
    }
    printf("beq | rs1=%d, rs2=%d, imm=%d | regs[RPC](x)=%.2x\n", rs1, rs2, imm, regs[RPC]+4);
}

// branch if not equal
void BNE(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if (regs[rs1] != regs[rs2]) {
        regs[RPC] = (regs[RPC] - 4) + (imm << 1);
    }
    printf("bne | rs1=%d, rs2=%d, imm=%d | regs[RPC](x)=%.2x\n", rs1, rs2, imm, regs[RPC]+4);
}

// branch if less than, signed
void BLT(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if ((int32_t) regs[rs1] < (int32_t) regs[rs2]) {
        regs[RPC] = (regs[RPC] - 4) + (imm << 1);
    }
    printf("blt | rs1=%d, rs2=%d, imm=%d | regs[RPC](x)=%.2x\n", rs1, rs2, imm, regs[RPC]+4);
}

// branch if less than, unsigned
void BLTU(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if (regs[rs1] < regs[rs2]) {
        regs[RPC] = (regs[RPC] - 4) + (imm << 1);
    }
    printf("bltu | rs1=%d, rs2=%d, imm=%d | regs[RPC](x)=%.2x\n", rs1, rs2, imm, regs[RPC]+4);
}

// branch if Greater Than or Equal
void BGE(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if ((int32_t) regs[rs1] >= (int32_t) regs[rs2]) {
        regs[RPC] = (regs[RPC] - 4) + (imm << 1);
    }
    printf("bge | rs1=%d, rs2=%d, imm=%d | regs[RPC](x)=%.2x\n", rs1, rs2, imm, regs[RPC]+4);
}

// branch if Greater Than or Equal, unsigned
void BGEU(uint32_t rs1, uint32_t rs2, uint32_t imm) {
    if (regs[rs1] >= regs[rs2]) {
        regs[RPC] = (regs[RPC] - 4) + (imm << 1);
    }
    printf("bgeu | rs1=%d, rs2=%d, imm=%d | regs[RPC](x)=%.2x\n", rs1, rs2, imm, regs[RPC]+4);
}   

void LUI(uint32_t rd, uint32_t imm) {
    regs[rd] = imm << 12; // sign extend to 32 bits (upper from reg, lower to 0)
    printf("lui | rd=%d, imm=%.2x, regs[rd] = %d\n", rd, imm, regs[rd]);
}

void JAL(uint32_t rd, uint32_t imm) { 
    regs[rd] = regs[RPC] + 4; // save PC value of next instruc
    regs[RPC] = (regs[RPC] - 4) + (imm << 1); // jump to target (and no need to incremnent 4 to PC)
    printf("jal | rd=%d, regs[rd]=%d, imm = %d | regs[RPC](x) = %.2x\n", rd, regs[rd], imm, regs[RPC]+4);
}

// Jump to target code address from a register, and save the PC value of next instruction into a register.
void JALR(uint32_t rd, uint32_t rs1, uint32_t imm) {
    regs[rd] = regs[RPC] + 4;
    regs[RPC] = regs[rs1] + imm - 4; // and no need to incremnent 4 to PC
    printf("jalr rd=%d, regs[rs1]=%d, imm=%d | regs[RPC](x)=%.2x | regs[rd]=%d\n", rd, regs[rs1], imm, regs[RPC]+4, regs[rd]);
}