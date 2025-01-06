// Auto-generated RISC-V instruction encoding file
// DO NOT EDIT MANUALLY

#pragma once
#include <cstdint>
#include <cstddef>

namespace rvpin {
namespace encoding {

struct InstructionEncoding {
    const char* name;
    uint32_t mask;
    uint32_t match;
    const char* extension;
};

constexpr InstructionEncoding INSTRUCTION_ENCODINGS[] = {
    {"lui", 0x0000007f, 0x00000037, "RV_I"},
    {"auipc", 0x0000007f, 0x00000017, "RV_I"},
    {"jal", 0x0000007f, 0x0000006f, "RV_I"},
    {"jalr", 0x0000707f, 0x00000067, "RV_I"},
    {"beq", 0x0000707f, 0x00000063, "RV_I"},
    {"bne", 0x0000707f, 0x00001063, "RV_I"},
    {"blt", 0x0000707f, 0x00004063, "RV_I"},
    {"bge", 0x0000707f, 0x00005063, "RV_I"},
    {"bltu", 0x0000707f, 0x00006063, "RV_I"},
    {"bgeu", 0x0000707f, 0x00007063, "RV_I"},
    {"lb", 0x0000707f, 0x00000003, "RV_I"},
    {"lh", 0x0000707f, 0x00001003, "RV_I"},
    {"lw", 0x0000707f, 0x00002003, "RV_I"},
    {"lbu", 0x0000707f, 0x00004003, "RV_I"},
    {"lhu", 0x0000707f, 0x00005003, "RV_I"},
    {"sb", 0x0000707f, 0x00000023, "RV_I"},
    {"sh", 0x0000707f, 0x00001023, "RV_I"},
    {"sw", 0x0000707f, 0x00002023, "RV_I"},
    {"addi", 0x0000707f, 0x00000013, "RV_I"},
    {"slti", 0x0000707f, 0x00002013, "RV_I"},
    {"sltiu", 0x0000707f, 0x00003013, "RV_I"},
    {"xori", 0x0000707f, 0x00004013, "RV_I"},
    {"ori", 0x0000707f, 0x00006013, "RV_I"},
    {"andi", 0x0000707f, 0x00007013, "RV_I"},
    {"add", 0xfe00707f, 0x00000033, "RV_I"},
    {"sub", 0xfe00707f, 0x40000033, "RV_I"},
    {"sll", 0xfe00707f, 0x00001033, "RV_I"},
    {"slt", 0xfe00707f, 0x00002033, "RV_I"},
    {"sltu", 0xfe00707f, 0x00003033, "RV_I"},
    {"xor", 0xfe00707f, 0x00004033, "RV_I"},
    {"srl", 0xfe00707f, 0x00005033, "RV_I"},
    {"sra", 0xfe00707f, 0x40005033, "RV_I"},
    {"or", 0xfe00707f, 0x00006033, "RV_I"},
    {"and", 0xfe00707f, 0x00007033, "RV_I"},
    {"fence", 0x0000707f, 0x0000000f, "RV_I"},
    {"ecall", 0xffffffff, 0x00000073, "RV_I"},
    {"ebreak", 0xffffffff, 0x00100073, "RV_I"},
    {"fence_tso", 0xfff0707f, 0x8330000f, "RV_I"},
    {"pause", 0xffffffff, 0x0100000f, "RV_I"},
    {"scall", 0xffffffff, 0x00000073, "RV_I"},
    {"sbreak", 0xffffffff, 0x00100073, "RV_I"},
    {"mv", 0xfff0707f, 0x00000013, "RV_I"},
    {"neg", 0xfff0707f, 0x40000033, "RV_I"},
    {"nop", 0xffffffff, 0x00000013, "RV_I"},
    {"zext_b", 0xfff0707f, 0x00007013, "RV_I"},
    {"ret", 0xffffffff, 0x00008067, "RV_I"},
    {"bleu", 0x0000707f, 0x00007063, "RV_I"},
    {"bgtu", 0x0000707f, 0x00006063, "RV_I"},
    {"ble", 0x0000707f, 0x00005063, "RV_I"},
    {"bgez", 0x01f0707f, 0x00005063, "RV_I"},
    {"blez", 0x000ff07f, 0x00005063, "RV_I"},
    {"bgt", 0x0000707f, 0x00004063, "RV_I"},
    {"bgtz", 0x000ff07f, 0x00004063, "RV_I"},
    {"bltz", 0x01f0707f, 0x00004063, "RV_I"},
    {"bnez", 0x01f0707f, 0x00001063, "RV_I"},
    {"beqz", 0x01f0707f, 0x00000063, "RV_I"},
    {"seqz", 0xfff0707f, 0x00103013, "RV_I"},
    {"snez", 0xfe0ff07f, 0x00003033, "RV_I"},
    {"sltz", 0xfff0707f, 0x00002033, "RV_I"},
    {"sgtz", 0xfe0ff07f, 0x00002033, "RV_I"},
    {"jalr_pseudo", 0xfff07fff, 0x000000e7, "RV_I"},
    {"jr", 0xfff07fff, 0x00000067, "RV_I"},
    {"jal_pseudo", 0x00000fff, 0x000000ef, "RV_I"},
    {"j", 0x00000fff, 0x0000006f, "RV_I"},
    {"slli", 0xfe00707f, 0x00001013, "RV32_I"},
    {"srli", 0xfe00707f, 0x00005013, "RV32_I"},
    {"srai", 0xfe00707f, 0x40005013, "RV32_I"},
    {"slli_rv32", 0xfe00707f, 0x00001013, "RV32_I"},
    {"srli_rv32", 0xfe00707f, 0x00005013, "RV32_I"},
    {"srai_rv32", 0xfe00707f, 0x40005013, "RV32_I"}
};

constexpr size_t NUM_ENCODINGS = sizeof(INSTRUCTION_ENCODINGS) / sizeof(InstructionEncoding);

} // namespace encoding
} // namespace rvpin
