#pragma once

#include <functional>
#include "../core/instruction.hpp"

namespace rvpin {
namespace api {

// Main API for tool writers
class InstrumentationTool {
public:
    virtual ~InstrumentationTool() = default;
    
    // Called before program execution starts
    virtual void onProgramStart() {}
    
    // Called before each instruction
    virtual void onBeforeInstruction(const Instruction& inst) {}
    
    // Called after each instruction
    virtual void onAfterInstruction(const Instruction& inst) {}
    
    // Called when program execution ends
    virtual void onProgramEnd() {}
};

// Helper functions for common instrumentation tasks
namespace utils {
    // Get the current program counter
    uint64_t getProgramCounter();
    
    // Read/Write memory at given address
    uint64_t readMemory(uint64_t addr, size_t size);
    void writeMemory(uint64_t addr, uint64_t value, size_t size);
    
    // Get register value
    uint64_t getRegisterValue(uint32_t reg);
}

} // namespace api
} // namespace rvpin
