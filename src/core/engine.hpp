#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <string>
#include "decoder.hpp"

namespace rvpin {

class Engine {
public:
    using InstrumentationCallback = std::function<void(const Instruction&)>;
    
    static Engine& getInstance();
    
    // Initialize the instrumentation engine
    bool initialize(int argc, char* argv[]);
    
    // Register callbacks for different instrumentation points
    void registerBeforeInstruction(InstrumentationCallback callback);
    void registerAfterInstruction(InstrumentationCallback callback);
    
    // Start the instrumented program
    int run();
    
    // Get the current instruction being executed
    const Instruction* getCurrentInstruction() const;
    
private:
    Engine() = default;
    ~Engine() = default;
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
    
    std::string program_path_;
    std::unique_ptr<Decoder> decoder_;
    std::vector<InstrumentationCallback> before_callbacks_;
    std::vector<InstrumentationCallback> after_callbacks_;
    
    // Internal state
    std::unique_ptr<Instruction> current_instruction_;
};

} // namespace rvpin
