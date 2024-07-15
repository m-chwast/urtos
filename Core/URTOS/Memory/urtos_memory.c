#include <stddef.h>
#include "urtos_memory.h"
#include "urtos_config.h"


#define MEMORY_BLOCK_EMPTY_ADDR		0xFFFF


typedef struct URTOS_Memory_BlockHeader {
	uint16_t blockSize;
	uint16_t nextBlockIndex;
} BlockHeader;

typedef struct {
	uint16_t lastBlock;
	uint16_t freeSpace;
} BlockAllocator;


static uint16_t firstBlockIndex;

static uint8_t memory[URTOS_CONFIG_MEMORY_ALLOCATED_SIZE];


static inline uint16_t GetNextMemoryBlock(uint16_t previousBlock) {
	BlockHeader* prev = (BlockHeader*)&memory[previousBlock];
	uint16_t nextBlock = prev->nextBlockIndex;
	return nextBlock;
}

static inline BlockHeader* GetBlockHeader(uint16_t blockAddr) {
	return (BlockHeader*)&memory[blockAddr];
}

static BlockAllocator GetNextFreeSpace(uint16_t startBlock, uint16_t space) {
	// TODO
	uint16_t freeSpaceAddr = MEMORY_BLOCK_EMPTY_ADDR;
	uint16_t currentBlock = startBlock;
	uint16_t lastBlock = startBlock;

	while(currentBlock != MEMORY_BLOCK_EMPTY_ADDR) {

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

	BlockAllocator blockAllocator = GetNextFreeSpace(firstBlockIndex, bytesToAllocate);

	// allocation failure
	if(blockAllocator.freeSpace == MEMORY_BLOCK_EMPTY_ADDR) {
		return NULL;
	}

	const uint16_t freeSpaceAddr = blockAllocator.freeSpace;

	BlockHeader* lastHeader = GetBlockHeader(blockAllocator.lastBlock);
	uint16_t nextBlock = lastHeader->nextBlockIndex;
	lastHeader->nextBlockIndex = freeSpaceAddr;

	BlockHeader* currentHeader = GetBlockHeader(freeSpaceAddr);
	currentHeader->blockSize = bytesToAllocate;
	currentHeader->nextBlockIndex = nextBlock;

	void* result = currentHeader + sizeof(BlockHeader);
	return result;
}
