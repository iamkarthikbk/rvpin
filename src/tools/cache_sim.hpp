#pragma once

#include <vector>
#include <cstdint>
#include <unordered_map>
#include <list>
#include <memory>
#include <chrono>
#include "tools/champsim_tracer.hpp"

namespace rvpin {
namespace tools {

struct CacheConfig {
    uint32_t line_size;        // Cache line size in bytes
    uint32_t size;            // Total cache size in bytes
    uint32_t associativity;   // Number of ways
    bool write_back;          // Write-back or write-through
    bool write_allocate;      // Write-allocate or write-no-allocate
    std::string trace_file;   // Optional ChampSim trace file
};

struct CacheStats {
    uint64_t reads{0};
    uint64_t writes{0};
    uint64_t read_hits{0};
    uint64_t write_hits{0};
    uint64_t read_misses{0};
    uint64_t write_misses{0};
    uint64_t evictions{0};
    
    void clear() {
        reads = writes = read_hits = write_hits = 0;
        read_misses = write_misses = evictions = 0;
    }
};

// Cache line state
class CacheLine {
public:
    CacheLine(uint32_t line_size) : data_(line_size) {}
    
    uint64_t tag{0};
    bool valid{false};
    bool dirty{false};
    uint64_t last_access{0};
    std::vector<uint8_t> data_;
};

// Cache set (group of ways)
class CacheSet {
public:
    CacheSet(uint32_t associativity, uint32_t line_size) {
        ways_.reserve(associativity);
        for (uint32_t i = 0; i < associativity; i++) {
            ways_.emplace_back(std::make_unique<CacheLine>(line_size));
        }
    }
    
    // Allow move construction and assignment
    CacheSet(CacheSet&&) = default;
    CacheSet& operator=(CacheSet&&) = default;
    
    // Prevent copying
    CacheSet(const CacheSet&) = delete;
    CacheSet& operator=(const CacheSet&) = delete;
    
    // Returns nullptr if no hit
    CacheLine* findLine(uint64_t tag) {
        for (auto& way : ways_) {
            if (way->valid && way->tag == tag) {
                return way.get();
            }
        }
        return nullptr;
    }
    
    // Returns evicted line if any
    std::unique_ptr<CacheLine> insertLine(uint64_t tag, uint32_t line_size) {
        // First try to find an invalid line
        for (auto& way : ways_) {
            if (!way->valid) {
                way->tag = tag;
                way->valid = true;
                way->dirty = false;
                way->last_access = getCurrentTime();
                return nullptr;
            }
        }
        
        // Need to evict - find LRU
        size_t lru_idx = 0;
        uint64_t oldest_access = ways_[0]->last_access;
        
        for (size_t i = 1; i < ways_.size(); i++) {
            if (ways_[i]->last_access < oldest_access) {
                oldest_access = ways_[i]->last_access;
                lru_idx = i;
            }
        }
        
        // Create new line
        auto new_line = std::make_unique<CacheLine>(line_size);
        new_line->tag = tag;
        new_line->valid = true;
        new_line->dirty = false;
        new_line->last_access = getCurrentTime();
        
        // Swap with LRU line
        std::unique_ptr<CacheLine> evicted = std::move(ways_[lru_idx]);
        ways_[lru_idx] = std::move(new_line);
        
        return evicted;
    }
    
private:
    uint64_t getCurrentTime() {
        return std::chrono::steady_clock::now().time_since_epoch().count();
    }
    
    std::vector<std::unique_ptr<CacheLine>> ways_;
};

class Cache {
public:
    Cache(const CacheConfig& config) 
        : config_(config),
          num_sets_((config.size / config.line_size) / config.associativity) {
        sets_.reserve(num_sets_);
        for (uint32_t i = 0; i < num_sets_; i++) {
            sets_.emplace_back(config.associativity, config.line_size);
        }
        stats_.clear();

        // Initialize ChampSim tracer if trace file is specified
        if (!config.trace_file.empty()) {
            tracer_ = std::make_unique<ChampSimTracer>(config.trace_file);
        }
    }
    
    // Allow move construction and assignment
    Cache(Cache&&) = default;
    Cache& operator=(Cache&&) = default;
    
    // Prevent copying
    Cache(const Cache&) = delete;
    Cache& operator=(const Cache&) = delete;
    
    void access(uint64_t pc, uint64_t addr, bool is_write, uint32_t size) {
        // Record access in ChampSim trace if enabled
        if (tracer_) {
            tracer_->recordAccess(pc, addr, is_write, size);
        }

        uint64_t line_addr = addr / config_.line_size;
        uint64_t tag = line_addr / num_sets_;
        uint64_t set_idx = line_addr % num_sets_;
        
        // Record access time and pattern
        access_pattern_.push_back({addr, getCurrentTime(), is_write});
        
        if (is_write) {
            stats_.writes++;
        } else {
            stats_.reads++;
        }
        
        CacheLine* line = sets_[set_idx].findLine(tag);
        
        if (line) {  // Cache hit
            line->last_access = getCurrentTime();
            if (is_write) {
                line->dirty = true;
                stats_.write_hits++;
            } else {
                stats_.read_hits++;
            }
        } else {  // Cache miss
            if (is_write) {
                stats_.write_misses++;
            } else {
                stats_.read_misses++;
            }
            
            if (is_write && !config_.write_allocate) {
                // Write-no-allocate: Don't cache the line
                return;
            }
            
            auto evicted = sets_[set_idx].insertLine(tag, config_.line_size);
            if (evicted) {
                stats_.evictions++;
                if (evicted->dirty && config_.write_back) {
                    // Would write back to memory here
                }
            }
            
            if (is_write) {
                line = sets_[set_idx].findLine(tag);
                line->dirty = true;
            }
        }
    }
    
    const CacheStats& getStats() const { return stats_; }
    
    struct AccessRecord {
        uint64_t address;
        uint64_t time;
        bool is_write;
    };
    
    const std::vector<AccessRecord>& getAccessPattern() const {
        return access_pattern_;
    }
    
private:
    uint64_t getCurrentTime() {
        return std::chrono::steady_clock::now().time_since_epoch().count();
    }
    
    CacheConfig config_;
    uint32_t num_sets_;
    std::vector<CacheSet> sets_;
    CacheStats stats_;
    std::vector<AccessRecord> access_pattern_;
    std::unique_ptr<ChampSimTracer> tracer_;
};

} // namespace tools
} // namespace rvpin
