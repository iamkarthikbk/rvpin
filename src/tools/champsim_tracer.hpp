#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <cstdint>
#include <memory>
#include "tools/cache_sim.hpp"

namespace rvpin {
namespace tools {

class ChampSimTracer {
public:
    ChampSimTracer(const std::string& trace_file) 
        : trace_file_(trace_file) {
        trace_stream_.open(trace_file_, std::ios::binary | std::ios::out);
        if (!trace_stream_) {
            throw std::runtime_error("Failed to open trace file: " + trace_file);
        }
    }

    ~ChampSimTracer() {
        if (trace_stream_.is_open()) {
            trace_stream_.close();
        }
    }

    // Record a memory access for ChampSim trace
    void recordAccess(uint64_t pc, uint64_t addr, bool is_write, uint32_t size) {
        // ChampSim trace format:
        // uint64_t cycle_count
        // uint64_t instruction_pointer
        // uint64_t virtual_address
        // uint8_t is_write
        // uint32_t write_size
        
        uint64_t cycle = getCycleCount();
        trace_stream_.write(reinterpret_cast<char*>(&cycle), sizeof(cycle));
        trace_stream_.write(reinterpret_cast<char*>(&pc), sizeof(pc));
        trace_stream_.write(reinterpret_cast<char*>(&addr), sizeof(addr));
        uint8_t write = is_write ? 1 : 0;
        trace_stream_.write(reinterpret_cast<char*>(&write), sizeof(write));
        trace_stream_.write(reinterpret_cast<char*>(&size), sizeof(size));
    }

    // Get the trace file path
    std::string getTraceFile() const {
        return trace_file_;
    }

private:
    uint64_t getCycleCount() {
        return cycle_count_++;
    }

    std::string trace_file_;
    std::ofstream trace_stream_;
    uint64_t cycle_count_{0};
};

} // namespace tools
} // namespace rvpin
