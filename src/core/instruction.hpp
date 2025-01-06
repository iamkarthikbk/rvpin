#pragma once

#include <cstdint>
#include <string>

namespace rvpin {

class Instruction {
public:
    enum class Type {
        R_TYPE,
        I_TYPE,
        S_TYPE,
        B_TYPE,
        U_TYPE,
        J_TYPE,
        UNKNOWN
    };

    Instruction(uint32_t raw_inst);
    
    uint32_t getRawInstruction() const { return raw_inst_; }
    Type getType() const { return type_; }
    std::string getMnemonic() const { return mnemonic_; }
    
    // RISC-V specific instruction fields
    uint32_t getOpcode() const;
    uint32_t getFunct3() const;
    uint32_t getFunct7() const;
    uint32_t getRd() const;
    uint32_t getRs1() const;
    uint32_t getRs2() const;
    
private:
    uint32_t raw_inst_;
    Type type_;
    std::string mnemonic_;
    
    void decode();
};

} // namespace rvpin
