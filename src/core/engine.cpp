#include "engine.hpp"
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <vector>

namespace rvpin {

Engine& Engine::getInstance() {
    static Engine instance;
    return instance;
}

bool Engine::initialize(int argc, char* argv[]) {
    if (argc < 1) {
        return false;
    }
    
    program_path_ = argv[0];
    decoder_ = std::make_unique<Decoder>();
    return true;
}

void Engine::registerBeforeInstruction(InstrumentationCallback callback) {
    before_callbacks_.push_back(std::move(callback));
}

void Engine::registerAfterInstruction(InstrumentationCallback callback) {
    after_callbacks_.push_back(std::move(callback));
}

const Instruction* Engine::getCurrentInstruction() const {
    return current_instruction_.get();
}

int Engine::run() {
    // Load the program
    std::ifstream program(program_path_, std::ios::binary);
    if (!program) {
        std::cerr << "Failed to open program: " << program_path_ << "\n";
        return 1;
    }

    // Read program into memory
    std::vector<uint32_t> instructions;
    uint32_t inst;
    while (program.read(reinterpret_cast<char*>(&inst), sizeof(inst))) {
        instructions.push_back(inst);
    }

    // Execute each instruction
    for (uint32_t inst : instructions) {
        current_instruction_ = std::make_unique<Instruction>(inst);
        
        // Call before callbacks
        for (const auto& callback : before_callbacks_) {
            callback(*current_instruction_);
        }
        
        // TODO: Actually execute the instruction
        
        // Call after callbacks
        for (const auto& callback : after_callbacks_) {
            callback(*current_instruction_);
        }
    }

    return 0;
}

} // namespace rvpin
