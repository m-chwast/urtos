#include <gtest/gtest.h>

extern "C" {
#include "URTOS/Memory/urtos_memory.h"
#include "URTOS/Memory/urtos_memory_storage.h"
}


class MemoryFreeTests : public testing::Test {
	void SetUp() override {
		memset(memory, 0, sizeof(memory));
		firstBlock = NULL;
	}
};

std::array<uint8_t, sizeof(memory)> GetMemoryCopy() {
	std::array<uint8_t, sizeof(memory)> memoryCopy;
	for(unsigned i = 0; i < sizeof(memory); i++) {
		memoryCopy[i] = memory[i];
	}
	return memoryCopy;
}

TEST_F(MemoryFreeTests, DoesNothingOnNullPointer) {
	// allocate some initial values
	URTOS_Memory_Allocate(1);
	URTOS_Memory_Allocate(1);
	// store current memory state
	std::array<uint8_t, sizeof(memory)> initialMemory = GetMemoryCopy();
	BlockHeader* initialFirstBlock = firstBlock;

	// free null pointer
	URTOS_Memory_Free(NULL);

	std::array<uint8_t, sizeof(memory)> currentMemory = GetMemoryCopy();

	EXPECT_EQ(initialFirstBlock, firstBlock);
	EXPECT_EQ(initialMemory, currentMemory);
}

TEST_F(MemoryFreeTests, FreesOnlyAllocatedBlock) {
	void* block = URTOS_Memory_Allocate(1);
	URTOS_Memory_Free(block);
	EXPECT_EQ(firstBlock, nullptr);
}

TEST_F(MemoryFreeTests, FreeClearsMemoryOfFirstAllocatedBlock) {
	// allocate 2 blocks
	void* block = URTOS_Memory_Allocate(1);
	URTOS_Memory_Allocate(1);

	// clear first block
	auto expectedMemory = GetMemoryCopy();
	for(int i = 0; i < 12; i++) {
		expectedMemory[i] = 0;
	}

	URTOS_Memory_Free(block);

	auto currentMemory = GetMemoryCopy();

	EXPECT_EQ(currentMemory, expectedMemory);
}
