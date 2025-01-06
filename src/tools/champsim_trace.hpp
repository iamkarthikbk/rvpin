#pragma once

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include "champsim/cache.h"
#include "champsim/dram_controller.h"
#include "champsim/ooo_cpu.h"

namespace rvpin {
namespace tools {

class ChampSimTracer {
public:
    ChampSimTracer(const std::string& trace_file) 
        : trace_file_(trace_file) {
        trace_out_.open(trace_file_, std::ios::binary);
    }

    ~ChampSimTracer() {
        if (trace_out_.is_open()) {
            trace_out_.close();
        }
    }

    void onInstruction(uint64_t ip, bool is_branch, bool branch_taken, uint64_t target) {
        trace_entry_.ip = ip;
        trace_entry_.is_branch = is_branch;
        trace_entry_.branch_taken = branch_taken;
        trace_entry_.target = target;
        
        // Write to trace file if we have a complete entry
        if (has_memory_access_) {
            writeTraceEntry();
            has_memory_access_ = false;
        }
    }

    void onMemoryAccess(uint64_t addr, uint8_t size, bool is_write) {
        trace_entry_.addr = addr;
        trace_entry_.size = size;
        trace_entry_.type = is_write ? 1 : 0;
        has_memory_access_ = true;
        
        // Write to trace file if we have instruction info
        if (trace_entry_.ip != 0) {
            writeTraceEntry();
            trace_entry_.ip = 0;
            has_memory_access_ = false;
        }
    }

private:
    struct ChampSimTraceEntry {
        uint64_t ip = 0;           // Instruction pointer
        uint64_t target = 0;       // Branch target
        uint8_t is_branch = 0;     // Is this a branch?
        uint8_t branch_taken = 0;  // Was the branch taken?
        uint8_t size = 0;         // Size of memory access
        uint8_t type = 0;         // Type of access (0=read, 1=write)
        uint64_t addr = 0;        // Memory address accessed
    };

    void writeTraceEntry() {
        trace_out_.write(reinterpret_cast<char*>(&trace_entry_), 
                        sizeof(ChampSimTraceEntry));
    }

    std::string trace_file_;
    std::ofstream trace_out_;
    ChampSimTraceEntry trace_entry_;
    bool has_memory_access_ = false;
};

} // namespace tools
} // namespace rvpin
