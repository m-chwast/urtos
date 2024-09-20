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

TEST_F(MemoryFreeTests, DoesNothingOnNullPointer) {
	// allocate some initial values
	URTOS_Memory_Allocate(1);
	URTOS_Memory_Allocate(1);
	// store current memory state
	std::array<uint8_t, sizeof(memory)> initialMemory;
	for(unsigned i = 0; i < sizeof(memory); i++) {
		initialMemory[i] = memory[i];
	}
	BlockHeader* initialFirstBlock = firstBlock;

	// free null pointer
	URTOS_Memory_Free(NULL);

	std::array<uint8_t, sizeof(memory)> currentMemory;
	for(unsigned i = 0; i < sizeof(memory); i++) {
		currentMemory[i] = memory[i];
	}

	EXPECT_EQ(initialFirstBlock, firstBlock);
	EXPECT_EQ(initialMemory, currentMemory);
}
