#include <gtest/gtest.h>
#include <cstdint>
#include <vector>
#include <cstdlib>
#include <climits>

// Forward declaration of the vulnerable function from CustomTiming.cpp
extern "C" void processDisplayIds(uint32_t dispIdCount);

class BufferOverflowTest : public ::testing::TestWithParam<uint32_t> {};

TEST_P(BufferOverflowTest, BufferReadsNeverExceedDeclaredLength) {
    // Invariant: Memory allocation size calculation must prevent integer overflow
    // and ensure buffer reads never exceed allocated length
    uint32_t dispIdCount = GetParam();
    
    // Use signal handler to catch potential crashes from buffer overflow
    signal(SIGSEGV, [](int sig) {
        FAIL() << "Segmentation fault detected - buffer overflow occurred";
    });
    
    // Call the actual production function with adversarial input
    ASSERT_NO_FATAL_FAILURE(processDisplayIds(dispIdCount));
}

INSTANTIATE_TEST_SUITE_P(
    AdversarialInputs,
    BufferOverflowTest,
    ::testing::Values(
        // Exact exploit case: Causes integer overflow in sizeof(NV_GPU_DISPLAYIDS)*dispIdCount
        UINT32_MAX / sizeof(uint32_t) + 1,
        // Boundary case: Maximum value that doesn't overflow with typical struct size
        UINT32_MAX / 16,
        // Valid input: Normal operation value
        10
    )
);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}