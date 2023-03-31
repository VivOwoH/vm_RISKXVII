#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdint.h>

uint32_t mem_read(uint32_t addr, int num_cell, uint32_t instruc);
uint32_t mem_write(uint32_t addr, uint32_t value, int num_cell, uint32_t instruc);

// decode
void type_R(uint32_t i);
void type_I(uint32_t i, uint32_t op);
void type_S(uint32_t i);
void type_SB(uint32_t i);
void type_U(uint32_t instruc);
void type_UJ(uint32_t instruc);

// type R
void ADD(uint32_t rd, uint32_t rs1, uint32_t rs2);
void SUB(uint32_t rd, uint32_t rs1, uint32_t rs2);
void SLL(uint32_t rd, uint32_t rs1, uint32_t rs2);
void SLT(uint32_t rd, uint32_t rs1, uint32_t rs2);
void SLTU(uint32_t rd, uint32_t rs1, uint32_t rs2);
void XOR(uint32_t rd, uint32_t rs1, uint32_t rs2);
void SRL(uint32_t rd, uint32_t rs1, uint32_t rs2);
void SRA(uint32_t rd, uint32_t rs1, uint32_t rs2);
void OR(uint32_t rd, uint32_t rs1, uint32_t rs2);
void AND(uint32_t rd, uint32_t rs1, uint32_t rs2);

// type I
void ADDI(uint32_t rd, uint32_t rs1, uint32_t imm);
void LB(uint32_t rd, uint32_t rs1, uint32_t imm, uint32_t instruc);
void JALR(uint32_t rd, uint32_t rs1, uint32_t imm);
void LH(uint32_t rd, uint32_t rs1, uint32_t imm, uint32_t instruc);
void SLTI(uint32_t rd, uint32_t rs1, uint32_t imm);
void LW(uint32_t rd, uint32_t rs1, uint32_t imm, uint32_t instruc);
void SLTIU(uint32_t rd, uint32_t rs1, uint32_t imm);
void XORI(uint32_t rd, uint32_t rs1, uint32_t imm);
void LBU(uint32_t rd, uint32_t rs1, uint32_t imm, uint32_t instruc);
void LHU(uint32_t rd, uint32_t rs1, uint32_t imm, uint32_t instruc);
void ORI(uint32_t rd, uint32_t rs1, uint32_t imm);
void ANDI(uint32_t rd, uint32_t rs1, uint32_t imm);

// type S
void SB(uint32_t rs1, uint32_t rs2, uint32_t imm, uint32_t instruc);
void SH(uint32_t rs1, uint32_t rs2, uint32_t imm, uint32_t instruc);
void SW(uint32_t rs1, uint32_t rs2, uint32_t imm, uint32_t instruc);

// type SB
void BEQ(uint32_t rs1, uint32_t rs2, uint32_t imm);
void BNE(uint32_t rs1, uint32_t rs2, uint32_t imm);
void BLT(uint32_t rs1, uint32_t rs2, uint32_t imm);
void BGE(uint32_t rs1, uint32_t rs2, uint32_t imm);
void BLTU(uint32_t rs1, uint32_t rs2, uint32_t imm);
void BGEU(uint32_t rs1, uint32_t rs2, uint32_t imm);

// type U
void LUI(uint32_t rd, uint32_t imm);

// type UJ
void JAL(uint32_t rd, uint32_t imm);

#endif