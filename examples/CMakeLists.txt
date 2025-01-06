# Cache test program
add_executable(cache_test cache_test.cpp)
target_compile_options(cache_test PRIVATE -O0)  # Disable optimizations
target_link_libraries(cache_test PRIVATE rvpin_core)

# Cache analyzer
add_executable(cache_analyzer cache_analyzer.cpp)
target_compile_options(cache_analyzer PRIVATE -O0)  # Disable optimizations
target_link_libraries(cache_analyzer PRIVATE rvpin_core)
