#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "core/engine.hpp"
#include "tools/cache_sim.hpp"

// For generating spectrograms
struct SpectrogramConfig {
    uint32_t time_bins;     // Number of time divisions
    uint32_t addr_bins;     // Number of address space divisions
    uint32_t color_max;     // Maximum intensity value
};

class CacheAnalyzer {
public:
    CacheAnalyzer(const rvpin::tools::CacheConfig& cache_config,
                 const SpectrogramConfig& spec_config)
        : cache_(cache_config), spec_config_(spec_config) {}
    
    void onMemoryAccess(uint64_t addr, bool is_write, uint32_t size) {
        cache_.access(addr, is_write, size);
    }
    
    void generateSpectrogram(const std::string& filename) {
        const auto& access_pattern = cache_.getAccessPattern();
        if (access_pattern.empty()) return;
        
        // Find time range
        uint64_t min_time = access_pattern[0].time;
        uint64_t max_time = access_pattern[0].time;
        uint64_t min_addr = access_pattern[0].address;
        uint64_t max_addr = access_pattern[0].address;
        
        for (const auto& access : access_pattern) {
            min_time = std::min(min_time, access.time);
            max_time = std::max(max_time, access.time);
            min_addr = std::min(min_addr, access.address);
            max_addr = std::max(max_addr, access.address);
        }
        
        // Create bins
        std::vector<std::vector<uint32_t>> bins(
            spec_config_.time_bins,
            std::vector<uint32_t>(spec_config_.addr_bins));
        
        // Fill bins
        double time_scale = static_cast<double>(spec_config_.time_bins) / (max_time - min_time);
        double addr_scale = static_cast<double>(spec_config_.addr_bins) / (max_addr - min_addr);
        
        for (const auto& access : access_pattern) {
            int time_bin = static_cast<int>((access.time - min_time) * time_scale);
            int addr_bin = static_cast<int>((access.address - min_addr) * addr_scale);
            
            time_bin = std::min(time_bin, static_cast<int>(spec_config_.time_bins) - 1);
            addr_bin = std::min(addr_bin, static_cast<int>(spec_config_.addr_bins) - 1);
            
            bins[time_bin][addr_bin]++;
        }
        
        // Normalize and write to PGM file
        std::ofstream out(filename);
        out << "P2\n" << spec_config_.time_bins << " " << spec_config_.addr_bins << "\n"
            << spec_config_.color_max << "\n";
        
        // Find maximum count for normalization
        uint32_t max_count = 0;
        for (const auto& row : bins) {
            max_count = std::max(max_count, *std::max_element(row.begin(), row.end()));
        }
        
        // Write normalized values
        for (int y = spec_config_.addr_bins - 1; y >= 0; y--) {
            for (uint32_t x = 0; x < spec_config_.time_bins; x++) {
                uint32_t normalized = static_cast<uint32_t>(
                    static_cast<double>(bins[x][y]) / max_count * spec_config_.color_max);
                out << normalized << " ";
            }
            out << "\n";
        }
    }
    
    void printStats() {
        const auto& stats = cache_.getStats();
        std::cout << "\nCache Statistics:\n";
        std::cout << "---------------\n";
        std::cout << "Reads: " << stats.reads << "\n";
        std::cout << "Writes: " << stats.writes << "\n";
        std::cout << "Read hits: " << stats.read_hits << "\n";
        std::cout << "Write hits: " << stats.write_hits << "\n";
        std::cout << "Read misses: " << stats.read_misses << "\n";
        std::cout << "Write misses: " << stats.write_misses << "\n";
        std::cout << "Evictions: " << stats.evictions << "\n";
        
        double read_hit_rate = static_cast<double>(stats.read_hits) / stats.reads * 100.0;
        double write_hit_rate = static_cast<double>(stats.write_hits) / stats.writes * 100.0;
        double overall_hit_rate = static_cast<double>(stats.read_hits + stats.write_hits) /
                                (stats.reads + stats.writes) * 100.0;
        
        std::cout << "\nHit Rates:\n";
        std::cout << "----------\n";
        std::cout << "Read hit rate: " << read_hit_rate << "%\n";
        std::cout << "Write hit rate: " << write_hit_rate << "%\n";
        std::cout << "Overall hit rate: " << overall_hit_rate << "%\n";
    }
    
private:
    rvpin::tools::Cache cache_;
    SpectrogramConfig spec_config_;
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <program>\n";
        return 1;
    }
    
    // Configure cache
    rvpin::tools::CacheConfig cache_config{
        .line_size = 64,        // 64-byte lines
        .size = 32 * 1024,      // 32KB cache
        .associativity = 8,     // 8-way set associative
        .write_back = true,
        .write_allocate = true
    };
    
    // Configure spectrogram
    SpectrogramConfig spec_config{
        .time_bins = 1000,      // 1000 time divisions
        .addr_bins = 1000,      // 1000 address space divisions
        .color_max = 255        // 8-bit grayscale
    };
    
    // Create analyzer
    CacheAnalyzer analyzer(cache_config, spec_config);
    
    // Initialize Spike and RVPin
    auto& engine = rvpin::Engine::getInstance();
    if (!engine.initialize(argc - 1, argv + 1)) {
        std::cerr << "Failed to initialize engine\n";
        return 1;
    }
    
    // Register memory access callback
    engine.registerMemoryAccess(
        [&analyzer](uint64_t addr, bool is_write, uint32_t size) {
            analyzer.onMemoryAccess(addr, is_write, size);
        });
    
    // Run the program
    if (engine.run() != 0) {
        std::cerr << "Failed to run program\n";
        return 1;
    }
    
    // Generate results
    analyzer.printStats();
    analyzer.generateSpectrogram("cache_access_pattern.pgm");
    
    std::cout << "\nGenerated spectrogram in 'cache_access_pattern.pgm'\n";
    return 0;
}
