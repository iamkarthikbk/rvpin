# RVPin - RISC-V Dynamic Binary Instrumentation Tool

RVPin is a dynamic binary instrumentation tool for RISC-V, inspired by Intel's Pin framework. It allows you to analyze and modify RISC-V programs at runtime by intercepting instructions and system calls.

## Features

- Instruction-level instrumentation
- System call interception and analysis
- Extensible API for creating custom analysis tools
- Support for RISC-V instruction decoding
- Non-intrusive binary analysis

## Prerequisites

- CMake (3.10 or later)
- C++17 compatible compiler
- RISC-V GNU Toolchain (`riscv64-unknown-elf-gcc`, `riscv64-unknown-elf-as`)

## Building

```bash
mkdir build
cd build
cmake ..
make
```

## Project Structure

- `src/core/`: Core instrumentation engine components
  - `instruction.*`: RISC-V instruction representation and decoding
  - `decoder.*`: Instruction decoder
  - `engine.*`: Main instrumentation engine
- `src/api/`: Public API for tool writers
  - `instrumentation.*`: Base classes and utilities for creating tools
- `examples/`: Example instrumentation tools
  - `syscall_tracer.cpp`: System call tracking tool
  - `instruction_counter.cpp`: Instruction counting tool
- `test/`: Test programs and utilities
  - `hello.S`: Simple RISC-V hello world program

## Usage

### Running the Syscall Tracer Example

The project includes a simple hello world program written in RISC-V assembly and a syscall tracer tool to analyze it. Here's how to run the example:

1. Build the project as described above:
```bash
mkdir build
cd build
cmake ..
make
```

2. The hello world program (`hello`) will be automatically built during the make process. It's a simple program that:
   - Prints "Hello, World!" to stdout using the `write` syscall
   - Exits using the `exit` syscall

3. Run the syscall tracer on the hello world program:
```bash
./syscall_tracer ./hello
```

4. You should see output similar to:
```
Syscall detected: 64    # write syscall
Syscall detected: 93    # exit syscall

Syscall Summary:
---------------
Total syscalls: 2
```

This demonstrates the syscall tracer intercepting and logging:
- The `write` syscall (64) used to print "Hello, World!"
- The `exit` syscall (93) used to terminate the program

## Creating Custom Tools

1. Create a new class inheriting from `rvpin::api::InstrumentationTool`
2. Override the callback methods you're interested in:
   - `onBeforeInstruction()`
   - `onAfterInstruction()`
   - `onProgramStart()`
   - `onProgramEnd()`
3. Register your tool with the RVPin engine

Example:
```cpp
class MyTool : public rvpin::api::InstrumentationTool {
    void onBeforeInstruction(const rvpin::Instruction& inst) override {
        // Your analysis code here
    }
};
```

## Current Limitations

- Basic instruction execution only
- Limited system call support
- No support for dynamic libraries
- No memory state tracking

## Contributing

Contributions are welcome! Please feel free to submit pull requests.
