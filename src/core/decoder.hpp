#pragma once

#include <memory>
#include <vector>
#include "instruction.hpp"

namespace rvpin {

class Decoder {
public:
    Decoder() = default;
    
    // Decode a single instruction
    std::unique_ptr<Instruction> decode(uint32_t raw_inst);
    
    // Decode a sequence of instructions
    std::vector<std::unique_ptr<Instruction>> decodeSequence(const uint32_t* instructions, size_t count);
    
    // Check if an address contains a valid instruction
    bool isValidInstruction(uint32_t raw_inst) const;
    
private:
    // Internal decoding tables and utilities
    void initDecodingTables();
};

} // namespace rvpin
