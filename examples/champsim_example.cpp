#include <iostream>
#include <string>
#include <random>
#include "tools/cache_sim.hpp"

using namespace rvpin::tools;

int main() {
    // Configure cache with ChampSim trace generation
    CacheConfig config{
        .line_size = 64,
        .size = 32768,  // 32KB
        .associativity = 8,
        .write_back = true,
        .write_allocate = true,
        .trace_file = "memory_trace.champsimtrace"
    };

    // Create cache instance
    Cache cache(config);

    // Generate some random memory accesses
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint64_t> addr_dist(0, 0xFFFFFFFF);  // 4GB address space
    std::uniform_int_distribution<uint64_t> pc_dist(0x400000, 0x500000);  // Typical program code range
    std::bernoulli_distribution write_dist(0.2);  // 20% writes

    const int NUM_ACCESSES = 1000000;
    
    std::cout << "Generating " << NUM_ACCESSES << " memory accesses..." << std::endl;

    // Simulate memory accesses
    for (int i = 0; i < NUM_ACCESSES; i++) {
        uint64_t addr = addr_dist(gen);
        uint64_t pc = pc_dist(gen);
        bool is_write = write_dist(gen);
        uint32_t size = 4;  // Assume 4-byte accesses

        cache.access(pc, addr, is_write, size);

        if (i % 100000 == 0) {
            std::cout << "Processed " << i << " accesses" << std::endl;
        }
    }

    // Print cache statistics
    const auto& stats = cache.getStats();
    std::cout << "\nCache Statistics:" << std::endl;
    std::cout << "Total Reads: " << stats.reads << std::endl;
    std::cout << "Total Writes: " << stats.writes << std::endl;
    std::cout << "Read Hits: " << stats.read_hits << std::endl;
    std::cout << "Write Hits: " << stats.write_hits << std::endl;
    std::cout << "Read Misses: " << stats.read_misses << std::endl;
    std::cout << "Write Misses: " << stats.write_misses << std::endl;
    std::cout << "Evictions: " << stats.evictions << std::endl;

    std::cout << "\nTrace file generated: " << config.trace_file << std::endl;
    std::cout << "Run ChampSim with: bin/champsim --warmup_instructions 200000000 --simulation_instructions 500000000 " 
              << config.trace_file << std::endl;

    return 0;
}
