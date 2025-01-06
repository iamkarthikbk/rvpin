cmake_minimum_required(VERSION 3.10)
project(rvpin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find RISC-V toolchain
find_program(RISCV_GCC riscv64-unknown-elf-gcc)
find_program(RISCV_AS riscv64-unknown-elf-as)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Main library
add_library(rvpin SHARED
    src/core/engine.cpp
    src/core/decoder.cpp
    src/core/instruction.cpp
    src/api/instrumentation.cpp
)

# Example tools
add_executable(instruction_counter
    examples/instruction_counter.cpp
)

add_executable(syscall_tracer
    examples/syscall_tracer.cpp
)

target_link_libraries(instruction_counter PRIVATE rvpin)
target_link_libraries(syscall_tracer PRIVATE rvpin)

# Test program
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/hello
    COMMAND ${RISCV_AS} -o ${CMAKE_BINARY_DIR}/hello ${CMAKE_SOURCE_DIR}/test/hello.S
    DEPENDS ${CMAKE_SOURCE_DIR}/test/hello.S
    COMMENT "Building RISC-V hello world program"
)

add_custom_target(hello ALL DEPENDS ${CMAKE_BINARY_DIR}/hello)

# Installation
install(TARGETS rvpin
    LIBRARY DESTINATION lib
    PUBLIC_HEADER DESTINATION include/rvpin
)
