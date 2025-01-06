#include "decoder.hpp"

namespace rvpin {

std::unique_ptr<Instruction> Decoder::decode(uint32_t raw_inst) {
    return std::make_unique<Instruction>(raw_inst);
}

std::vector<std::unique_ptr<Instruction>> Decoder::decodeSequence(
    const uint32_t* instructions, size_t count) {
    std::vector<std::unique_ptr<Instruction>> result;
    for (size_t i = 0; i < count; ++i) {
        result.push_back(decode(instructions[i]));
    }
    return result;
}

bool Decoder::isValidInstruction(uint32_t raw_inst) const {
    // Basic validation - check if it's not all zeros or all ones
    return raw_inst != 0 && raw_inst != 0xFFFFFFFF;
}

void Decoder::initDecodingTables() {
    // TODO: Initialize lookup tables for instruction decoding
}

} // namespace rvpin
