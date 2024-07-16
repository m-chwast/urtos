#include <stddef.h>
#include "urtos_memory.h"
#include "urtos_config.h"


typedef struct URTOS_Memory_BlockHeader {
	uint16_t blockSize;
	struct URTOS_Memory_BlockHeader* nextBlock;
} BlockHeader;

typedef struct {
	BlockHeader* lastBlock;
	BlockHeader* freeSpace;
} BlockAllocator;


static BlockHeader* firstBlock = NULL;

static uint8_t memory[URTOS_CONFIG_MEMORY_ALLOCATED_SIZE];


static inline BlockHeader* GetNextMemoryBlock(BlockHeader* previousBlock) {
	BlockHeader* nextBlock = previousBlock->nextBlock;
	return nextBlock;
}

static BlockAllocator GetNextFreeSpace(const BlockHeader* startBlock, uint16_t space) {
	// TODO
	BlockHeader* freeSpaceAddr = NULL;
	BlockHeader* currentBlock = startBlock;
	BlockHeader* lastBlock = startBlock;

	while(currentBlock != NULL) {

	}

	BlockAllocator allocator = {
			.freeSpace = freeSpaceAddr,
			.lastBlock = lastBlock,
	};
	return allocator;
}

void* URTOS_Memory_Allocate(uint16_t bytesToAllocate) {
	// TODO handling first block
	if(bytesToAllocate == 0) {
		return NULL;
	}

	BlockAllocator blockAllocator = GetNextFreeSpace(firstBlock, bytesToAllocate);
	BlockHeader* freeSpaceBlock = blockAllocator.freeSpace;
	BlockHeader* lastBlock = blockAllocator.lastBlock;

	// allocation failure
	if(freeSpaceBlock == NULL) {
		return NULL;
	}

	BlockHeader* nextBlock = lastBlock->nextBlock;
	lastBlock->nextBlock = freeSpaceBlock;

	freeSpaceBlock->blockSize = bytesToAllocate;
	freeSpaceBlock->nextBlock= nextBlock;

	void* result = freeSpaceBlock + sizeof(BlockHeader);
	return result;
}
