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
	void* res = URTOS_Memory_Allocate(URTOS_CONFIG_MEMORY_ALLOCATED_SIZE - 7);

	EXPECT_EQ(res, nullptr);
	EXPECT_EQ(firstBlock, nullptr);
}

TEST_F(MemoryAllocateTests, AllocatingMaxBlockWorks) {
	void* res = URTOS_Memory_Allocate(URTOS_CONFIG_MEMORY_ALLOCATED_SIZE - 8);

	EXPECT_EQ(res, &memory[8]);
}

TEST_F(MemoryAllocateTests, FirstAllocationWorks) {
	void* res = URTOS_Memory_Allocate(4);

	EXPECT_NE(res, nullptr);
	EXPECT_EQ((uint8_t*)firstBlock, memory);
	EXPECT_EQ(res, &memory[8]);
	EXPECT_EQ(firstBlock->blockSize, 4);
}

TEST_F(MemoryAllocateTests, AllocationOfNon4MultipleExtendsSize) {
	URTOS_Memory_Allocate(5);

	EXPECT_NE(firstBlock, nullptr);
	EXPECT_EQ(firstBlock->blockSize, 8);
}

TEST_F(MemoryAllocateTests, AllocationOfNon4MultipleExtendsSize2) {
	URTOS_Memory_Allocate(7);

	EXPECT_NE(firstBlock, nullptr);
	EXPECT_EQ(firstBlock->blockSize, 8);
}

TEST_F(MemoryAllocateTests, AllocationOfNon4MultipleExtendsSize3) {
	URTOS_Memory_Allocate(10);

	EXPECT_NE(firstBlock, nullptr);
	EXPECT_EQ(firstBlock->blockSize, 12);
}

TEST_F(MemoryAllocateTests, DualAllocationWorksWithCorrectValues) {
	void* res1 = URTOS_Memory_Allocate(10);
	void* res2 = URTOS_Memory_Allocate(7);

	EXPECT_EQ(firstBlock->blockSize, 12);

	BlockHeader* next = firstBlock->nextBlock;
	EXPECT_EQ((uint8_t*)next, &memory[20]);
	EXPECT_EQ(next->blockSize, 8);

	EXPECT_EQ(res1, &memory[8]);
	EXPECT_EQ(res2, &memory[28]);
}

TEST_F(MemoryAllocateTests, MultipleAllocationWorks) {
	void* res1 = URTOS_Memory_Allocate(4);
	void* res2 = URTOS_Memory_Allocate(5);
	void* res3 = URTOS_Memory_Allocate(6);
	void* res4 = URTOS_Memory_Allocate(7);

	EXPECT_EQ(res1, &memory[8]);
	EXPECT_EQ(res2, &memory[20]);
	EXPECT_EQ(res3, &memory[36]);
	EXPECT_EQ(res4, &memory[52]);
}

