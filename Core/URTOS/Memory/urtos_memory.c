#include <stddef.h>
#include "urtos_memory.h"
#include "urtos_config.h"


#define MEMORY_BLOCK_EMPTY_ADDR		0xFFFF


struct URTOS_Memory_BlockHeader {
	uint16_t blockSize;
	uint16_t nextBlockIndex;
};

typedef struct URTOS_Memory_BlockHeader BlockHeader;

static uint16_t firstBlockIndex;

static uint8_t memory[URTOS_CONFIG_MEMORY_ALLOCATED_SIZE];


static inline uint16_t GetNextMemoryBlock(uint16_t previousBlock) {
	BlockHeader* prev = (BlockHeader*)&memory[previousBlock];
	uint16_t nextBlock = prev->nextBlockIndex;
	return nextBlock;
}

static uint16_t GetNextFreeSpace(uint16_t startBlock, uint16_t space) {
	uint16_t freeSpaceAddr = MEMORY_BLOCK_EMPTY_ADDR;
	uint16_t currentBlock = startBlock;

	while(currentBlock != MEMORY_BLOCK_EMPTY_ADDR) {

	}

	return freeSpaceAddr;
}

void* URTOS_Memory_Allocate(uint16_t bytesToAllocate) {
	if(bytesToAllocate == 0) {
		return NULL;
	}

}
