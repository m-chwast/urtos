#include <stddef.h>
#include <string.h>
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

/*
 * Free space search algorithm:
 *
 * 1. Initialize current block and last block to start block
 * 2. Find the block that is closest to the current block from the right side:
 * 		example memory structure: |---A-C--ED--B-|, where A - current block;
 * 		executing the step will set last block to C
 *
 * 		Now we know that between A and C there is NO OTHER BLOCK - space between is empty and can be allocated.
 * 		We also know that for any other block on the right there is at least one allocated block (C), so we can't treat that as empty space.
 *
 *		If no block is found on the right, treat right memory border as the right block.
 *
 * 	3. Check the empty space size and perform the allocation if space is sufficient - "glue" the new block to right side of A - DONE
 * 	4. If space was not sufficient, set current block to its next block
 * 	5. If current block is not NULL then GOTO step 2
 * 	6. Else find leftmost block and check space between left memory border and found block
 * 	7. If able, allocate at the left memory border - DONE
 * 	8. Else - FAIL - no empty space with sufficient capacity
 *
 */

static BlockAllocator GetNextFreeSpace(const BlockHeader* startBlock, uint16_t space) {
	// TODO
	BlockHeader* freeSpaceAddr = NULL;
	BlockHeader* currentBlock = startBlock;
	BlockHeader* lastBlock = startBlock;

	if(currentBlock == NULL) {
		firstBlock = (BlockHeader*)&memory[0];
		memset(firstBlock, 0, sizeof(BlockHeader));
		freeSpaceAddr = firstBlock;
		lastBlock = firstBlock;
		currentBlock = NULL;
	}

	while(currentBlock != NULL) {

	}

	BlockAllocator allocator = {
			.freeSpace = freeSpaceAddr,
			.lastBlock = lastBlock,
	};
	return allocator;
}

void* URTOS_Memory_Allocate(uint16_t bytesToAllocate) {
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
