# Core library
add_library(rvpin_core
    core/engine.cpp
    core/decoder.cpp
    core/instruction.cpp
)

target_include_directories(rvpin_core PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_compile_features(rvpin_core PUBLIC cxx_std_17)
