#include "instruction.hpp"

namespace rvpin {

Instruction::Instruction(uint32_t raw_inst) : raw_inst_(raw_inst) {
    decode();
}

uint32_t Instruction::getOpcode() const {
    return raw_inst_ & 0x7F;
}

uint32_t Instruction::getFunct3() const {
    return (raw_inst_ >> 12) & 0x7;
}

uint32_t Instruction::getFunct7() const {
    return (raw_inst_ >> 25) & 0x7F;
}

uint32_t Instruction::getRd() const {
    return (raw_inst_ >> 7) & 0x1F;
}

uint32_t Instruction::getRs1() const {
    return (raw_inst_ >> 15) & 0x1F;
}

uint32_t Instruction::getRs2() const {
    return (raw_inst_ >> 20) & 0x1F;
}

void Instruction::decode() {
    uint32_t opcode = getOpcode();
    
    // Basic instruction type detection
    switch (opcode) {
        case 0x33: // R-type
            type_ = Type::R_TYPE;
            break;
        case 0x13: // I-type
            type_ = Type::I_TYPE;
            break;
        case 0x23: // S-type
            type_ = Type::S_TYPE;
            break;
        case 0x63: // B-type
            type_ = Type::B_TYPE;
            break;
        case 0x37: // U-type
        case 0x17:
            type_ = Type::U_TYPE;
            break;
        case 0x6F: // J-type
            type_ = Type::J_TYPE;
            break;
        default:
            type_ = Type::UNKNOWN;
    }
    
    // For demonstration, just use a simple mnemonic
    mnemonic_ = "UNKNOWN";
    if (raw_inst_ == 0x00000073) {
        mnemonic_ = "ECALL";
    }
}

} // namespace rvpin
