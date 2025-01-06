#include <iostream>
#include <iomanip>
#include "../src/api/instrumentation.hpp"
#include "../src/core/engine.hpp"

class SyscallTracer : public rvpin::api::InstrumentationTool {
public:
    void onBeforeInstruction(const rvpin::Instruction& inst) override {
        // Check if instruction is ecall
        if (inst.getRawInstruction() == 0x00000073) { // ECALL opcode
            uint64_t syscall_num = rvpin::api::utils::getRegisterValue(17); // a7 register
            std::cout << "Syscall detected: " << std::dec << syscall_num << "\n";
            syscall_count_++;
        }
    }
    
    void onProgramEnd() override {
        std::cout << "\nSyscall Summary:\n";
        std::cout << "---------------\n";
        std::cout << "Total syscalls: " << syscall_count_ << "\n";
    }
    
private:
    uint64_t syscall_count_ = 0;
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <program_to_instrument>\n";
        return 1;
    }

    auto& engine = rvpin::Engine::getInstance();
    SyscallTracer tracer;
    
    if (!engine.initialize(argc - 1, argv + 1)) {
        std::cerr << "Failed to initialize RVPin\n";
        return 1;
    }

    engine.registerBeforeInstruction([&tracer](const rvpin::Instruction& inst) {
        tracer.onBeforeInstruction(inst);
    });

    return engine.run();
}
