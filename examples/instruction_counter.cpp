#include "../src/api/instrumentation.hpp"
#include "../src/core/engine.hpp"
#include <iostream>
#include <unordered_map>
#include <iomanip>

class InstructionCounter : public rvpin::api::InstrumentationTool {
public:
    void onBeforeInstruction(const rvpin::Instruction& inst) override {
        std::string mnemonic = inst.getMnemonic();
        if (mnemonic == "UNKNOWN") {
            std::cout << "Unknown instruction: 0x" << std::hex 
                      << std::setw(8) << std::setfill('0') 
                      << inst.getRawInstruction() << std::dec << "\n";
            unknown_count_++;
        } else {
            instruction_count_[mnemonic]++;
        }
        total_instructions_++;
    }
    
    void onProgramEnd() override {
        std::cout << "\nInstruction Count Summary:\n";
        std::cout << "-------------------------\n";
        
        // Find the longest mnemonic for alignment
        size_t max_width = 0;
        for (const auto& [mnemonic, _] : instruction_count_) {
            max_width = std::max(max_width, mnemonic.length());
        }
        
        // Print counts with nice alignment
        for (const auto& [mnemonic, count] : instruction_count_) {
            std::cout << std::left << std::setw(max_width + 2) << mnemonic 
                      << ": " << count << "\n";
        }
        
        if (unknown_count_ > 0) {
            std::cout << "UNKNOWN: " << unknown_count_ << "\n";
        }
        
        std::cout << "-------------------------\n";
        std::cout << "Total Instructions: " << total_instructions_ << "\n";
    }
    
private:
    std::unordered_map<std::string, uint64_t> instruction_count_;
    uint64_t total_instructions_ = 0;
    uint64_t unknown_count_ = 0;
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <program_to_instrument>\n";
        return 1;
    }

    auto& engine = rvpin::Engine::getInstance();
    InstructionCounter counter;
    
    if (!engine.initialize(argc - 1, argv + 1)) {
        std::cerr << "Failed to initialize RVPin\n";
        return 1;
    }

    engine.registerBeforeInstruction([&counter](const rvpin::Instruction& inst) {
        counter.onBeforeInstruction(inst);
    });

    int result = engine.run();
    counter.onProgramEnd();
    return result;
}
