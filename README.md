# RVPin: RISC-V Dynamic Binary Instrumentation Tool

RVPin is a powerful Dynamic Binary Instrumentation (DBI) tool for RISC-V programs. It allows you to analyze and modify RISC-V programs while they're running, making it perfect for debugging, profiling, and program analysis.

## What is Dynamic Binary Instrumentation?

Dynamic Binary Instrumentation (DBI) is a technique that allows you to add code to a program while it's running. This is useful for:
- Counting how many times each instruction is used
- Tracking which functions are called
- Monitoring system calls
- Finding memory leaks and bugs
- And much more!

## Features

- **Instruction Analysis**: Track and analyze RISC-V instructions as they execute
- **Instrumentation API**: Easy-to-use API for adding your own analysis code
- **Built-in Tools**: Ready-to-use tools for common tasks:
  - Instruction Counter: See which instructions are used most often
  - System Call Tracer: Monitor interactions with the operating system

## Prerequisites

Before you start, make sure you have:

1. **C++ Development Tools**:
   - A C++17 compatible compiler (GCC or Clang)
   - CMake (version 3.10 or higher)
   - Make

2. **RISC-V Toolchain**:
   - RISC-V GNU Toolchain for compiling test programs
   - Instructions for installing the toolchain:
     ```bash
     # On Ubuntu/Debian:
     sudo apt-get install gcc-riscv64-linux-gnu

     # On macOS with Homebrew:
     brew install riscv-tools
     ```

3. **Python**:
   - Python 3.6 or higher
   - Required for running the setup scripts

## Getting Started

### 1. Clone the Repository
```bash
git clone https://github.com/iamkarthikbk/rvpin.git
cd rvpin
```

### 2. Run the Setup Script
```bash
# Make the script executable
chmod +x scripts/setup.sh

# Run the setup script
./scripts/setup.sh
```
This script will:
- Download necessary RISC-V instruction definitions
- Generate the instruction encoding files
- Build the project

### 3. Build the Project
```bash
# Create build directory
mkdir -p build
cd build

# Configure and build
cmake ..
make
```

### 4. Try the Example Tools

#### Instruction Counter
Count how many times each RISC-V instruction is used in a program:
```bash
# First, build the hello world test program
make hello

# Then run the instruction counter
./instruction_counter ./hello
```

You should see output like this:
```
Instruction Count Summary:
-------------------------
addi   : 6
ecall  : 2
auipc  : 1
-------------------------
Total Instructions: 9
```

#### System Call Tracer
Monitor system calls made by a program:
```bash
./syscall_tracer ./hello
```

## Writing Your Own Tools

You can create your own analysis tools using RVPin's API. Here's a simple example:

```cpp
#include "core/engine.hpp"

class MyTool {
public:
    void onBeforeInstruction(const rvpin::Instruction& inst) {
        // Your analysis code here
        // This runs before each instruction
    }
};

int main(int argc, char* argv[]) {
    auto& engine = rvpin::Engine::getInstance();
    
    // Initialize the engine with the target program
    if (!engine.initialize(argc - 1, argv + 1)) {
        return 1;
    }
    
    // Create your tool
    MyTool tool;
    
    // Register callbacks
    engine.registerBeforeInstruction(
        [&tool](const rvpin::Instruction& inst) {
            tool.onBeforeInstruction(inst);
        });
    
    // Run the instrumented program
    return engine.run();
}
```

## Project Structure

- `src/core/`: Core instrumentation engine
  - `engine.cpp`: Main instrumentation engine
  - `decoder.cpp`: RISC-V instruction decoder
  - `instruction.cpp`: Instruction representation
- `examples/`: Example tools
  - `instruction_counter.cpp`: Count instruction usage
  - `syscall_tracer.cpp`: Track system calls
- `scripts/`: Setup and utility scripts
  - `setup.sh`: Project setup script
  - `generate_encoding.py`: Generate instruction encodings

## Contributing

Contributions are welcome! Whether it's:
- Bug fixes
- New features
- Documentation improvements
- Tool improvements

Please feel free to:
1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Support

If you encounter any issues or have questions:
1. Check the [Issues](https://github.com/iamkarthikbk/rvpin/issues) page
2. Open a new issue if needed
3. Provide as much detail as possible about your problem

## Cache Simulator Setup

### Python Dependencies
The cache analyzer visualization requires Python 3.9+ and several dependencies. Install them using:

```bash
pip install -r requirements.txt
```

## Setup Instructions

### Python Setup
The cache analyzer visualization requires Python 3.9+ and several dependencies. Install them using:

```bash
pip install -r requirements.txt
```
