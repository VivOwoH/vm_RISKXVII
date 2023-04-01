#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdint.h>

// decode
void type_R(uint32_t i);
void type_I(uint32_t i, uint32_t op);
void type_S(uint32_t i);
void type_SB(uint32_t i);
void type_U(uint32_t instruc);
void type_UJ(uint32_t instruc);

void R_operation(int command, uint32_t rd, uint32_t rs1, uint32_t rs2);
void I_operation(int command, uint32_t rd, uint32_t rs1, uint32_t imm, uint32_t insruc);
void S_operation(int command, uint32_t rs1, uint32_t rs2, uint32_t imm, uint32_t instruc);
void SB_operation(int command, uint32_t rs1, uint32_t rs2, uint32_t imm);

// type R
enum { ADD, SUB, SLL, SLT, SLTU, XOR, SRL, SRA, OR, AND };

// type I
enum { ADDI, LB, JALR, LH, SLTI, LW, SLTIU, XORI, LBU, LHU, ORI, ANDI };

// type S
enum { SB, SH, SW };

// type SB
enum { BEQ, BNE, BLT, BGE, BLTU, BGEU };

// type U
void LUI(uint32_t rd, uint32_t imm);

// type UJ
void JAL(uint32_t rd, uint32_t imm);

#endif