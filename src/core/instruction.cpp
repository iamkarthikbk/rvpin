#include "instruction.hpp"
#include "encoding.hpp"
#include <sstream>

namespace rvpin {

Instruction::Instruction(uint32_t raw_inst) : raw_inst_(raw_inst), type_(Type::UNKNOWN) {
    decode();
}

void Instruction::decode() {
    // Try to match against known instruction encodings
    for (const auto& encoding : encoding::INSTRUCTION_ENCODINGS) {
        if ((raw_inst_ & encoding.mask) == (encoding.match & encoding.mask)) {
            mnemonic_ = encoding.name;
            return;
        }
    }

    // Special case for ECALL (until we get proper encoding)
    if (raw_inst_ == 0x00000073) {
        mnemonic_ = "ECALL";
        type_ = Type::I_TYPE;
        return;
    }

    // If no match found, mark as unknown
    mnemonic_ = "UNKNOWN";
    type_ = Type::UNKNOWN;
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

} // namespace rvpin
