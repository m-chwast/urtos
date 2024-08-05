#include <gtest/gtest.h>

extern "C" {
#include "URTOS/Memory/urtos_memory.h"
#include "URTOS/Memory/urtos_memory_storage.h"
}


class MemoryAllocateTests : public testing::Test {
	void SetUp() override {
		memset(memory, 0, sizeof(memory));
		firstBlock = NULL;
	}
};

TEST_F(MemoryAllocateTests, BlockHeaderTakesTwoBytes) {
	EXPECT_EQ(sizeof(BlockHeader), 8);
}

TEST_F(MemoryAllocateTests, AllocatingTooLargeBlockAtStartupFails) {
	void* res = URTOS_Memory_Allocate(URTOS_CONFIG_MEMORY_ALLOCATED_SIZE);

	EXPECT_EQ(res, nullptr);
	EXPECT_EQ(firstBlock, nullptr);
}
