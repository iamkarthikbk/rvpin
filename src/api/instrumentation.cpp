#include "instrumentation.hpp"

namespace rvpin {
namespace api {
namespace utils {

uint64_t getProgramCounter() {
    // TODO: Implement actual PC reading
    return 0;
}

uint64_t readMemory(uint64_t addr, size_t size) {
    // TODO: Implement memory reading
    return 0;
}

void writeMemory(uint64_t addr, uint64_t value, size_t size) {
    // TODO: Implement memory writing
}

uint64_t getRegisterValue(uint32_t reg) {
    // TODO: Implement register reading
    // For demonstration, return 64 for write syscall and 93 for exit syscall
    if (reg == 17) { // a7 register
        static bool first_call = true;
        if (first_call) {
            first_call = false;
            return 64; // write syscall
        }
        return 93; // exit syscall
    }
    return 0;
}

} // namespace utils
} // namespace api
} // namespace rvpin
