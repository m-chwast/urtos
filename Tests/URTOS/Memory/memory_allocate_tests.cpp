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
