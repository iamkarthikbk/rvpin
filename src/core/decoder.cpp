#include "decoder.hpp"
#include "encoding.hpp"

namespace rvpin {

std::unique_ptr<Instruction> Decoder::decode(uint32_t raw_inst) {
    if (!isValidInstruction(raw_inst)) {
        return nullptr;
    }
    return std::make_unique<Instruction>(raw_inst);
}

std::vector<std::unique_ptr<Instruction>> Decoder::decodeSequence(
    const uint32_t* instructions, size_t count) {
    std::vector<std::unique_ptr<Instruction>> result;
    for (size_t i = 0; i < count; ++i) {
        auto inst = decode(instructions[i]);
        if (inst) {
            result.push_back(std::move(inst));
        }
    }
    return result;
}

bool Decoder::isValidInstruction(uint32_t raw_inst) const {
    // Basic validation - check if it's not all zeros or all ones
    if (raw_inst == 0 || raw_inst == 0xFFFFFFFF) {
        return false;
    }
    
    // Check if the instruction matches any known encoding
    for (const auto& encoding : encoding::INSTRUCTION_ENCODINGS) {
        if ((raw_inst & encoding.mask) == (encoding.match & encoding.mask)) {
            return true;
        }
    }
    
    // Special case for ECALL
    if (raw_inst == 0x00000073) {
        return true;
    }
    
    // Check if it's a compressed instruction (16-bit)
    // TODO: Add support for compressed instructions
    
    return false;
}

void Decoder::initDecodingTables() {
    // TODO: Initialize lookup tables for instruction decoding
}

} // namespace rvpin
