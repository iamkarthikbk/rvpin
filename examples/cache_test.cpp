#include <cstdint>
#include <cstdlib>
#include <cstring>

// Array sizes chosen to create interesting cache patterns
#define ARRAY_SIZE (256 * 1024)  // 256KB - larger than typical L1
#define STRIDE_SIZE 64           // Common cache line size
#define BLOCK_SIZE 4096         // Common page size

// Global arrays to ensure they're in different cache sets
volatile uint8_t sequential_array[ARRAY_SIZE] __attribute__((aligned(4096)));
volatile uint8_t random_array[ARRAY_SIZE] __attribute__((aligned(4096)));
volatile uint8_t stride_array[ARRAY_SIZE] __attribute__((aligned(4096)));
volatile uint8_t matrix[512][512] __attribute__((aligned(4096)));  // 256KB matrix

// Prevent compiler optimization
volatile uint64_t sum = 0;

// Sequential access pattern - good spatial locality
void test_sequential_access() {
    for (int i = 0; i < ARRAY_SIZE; i++) {
        sum += sequential_array[i];
    }
}

// Random access pattern - poor spatial and temporal locality
void test_random_access() {
    for (int i = 0; i < ARRAY_SIZE; i++) {
        int idx = rand() % ARRAY_SIZE;
        sum += random_array[idx];
    }
}

// Strided access pattern - tests prefetcher effectiveness
void test_strided_access() {
    for (int stride = 1; stride <= STRIDE_SIZE; stride *= 2) {
        for (int i = 0; i < ARRAY_SIZE; i += stride) {
            sum += stride_array[i];
        }
    }
}

// Block access pattern - tests temporal locality
void test_block_access() {
    for (int block = 0; block < ARRAY_SIZE; block += BLOCK_SIZE) {
        for (int repeat = 0; repeat < 3; repeat++) {
            for (int i = 0; i < BLOCK_SIZE; i++) {
                sum += sequential_array[block + i];
            }
        }
    }
}

// Matrix operations - tests 2D spatial locality
void test_matrix_operations() {
    // Row-major access
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++) {
            sum += matrix[i][j];
        }
    }
    
    // Column-major access - typically worse cache performance
    for (int j = 0; j < 512; j++) {
        for (int i = 0; i < 512; i++) {
            sum += matrix[i][j];
        }
    }
    
    // Diagonal access - poor spatial locality
    for (int i = 0; i < 512; i++) {
        sum += matrix[i][i];
    }
}

// Thrashing test - accessing just over cache size with cyclic pattern
void test_cache_thrashing() {
    const int stride = 16384; // Larger than typical L1 cache
    for (int repeat = 0; repeat < 1000; repeat++) {
        for (int i = 0; i < ARRAY_SIZE; i += stride) {
            sum += sequential_array[i];
        }
    }
}

int main() {
    // Initialize arrays with some data
    for (int i = 0; i < ARRAY_SIZE; i++) {
        sequential_array[i] = i & 0xFF;
        random_array[i] = i & 0xFF;
        stride_array[i] = i & 0xFF;
    }
    
    for (int i = 0; i < 512; i++) {
        for (int j = 0; j < 512; j++) {
            matrix[i][j] = (i + j) & 0xFF;
        }
    }
    
    // Run tests
    test_sequential_access();  // Should show good cache performance
    test_random_access();      // Should show poor cache performance
    test_strided_access();     // Tests prefetcher
    test_block_access();       // Tests temporal locality
    test_matrix_operations();  // Tests 2D access patterns
    test_cache_thrashing();    // Tests cache replacement policy
    
    // Prevent optimization
    return sum & 1;
}
