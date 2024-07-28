#include <stddef.h>
#include <stdbool.h>
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

static const BlockHeader* GetClosestRightBlock(const BlockHeader* const block) {
	assert(block != NULL);
	assert(firstBlock != NULL);

	const BlockHeader* closestRightBlock = (const BlockHeader*)&memory[URTOS_CONFIG_MEMORY_ALLOCATED_SIZE - 1];

	// we will iterate over all blocks
	const BlockHeader* currentBlock = firstBlock;
	while(currentBlock != NULL) {
		if(currentBlock > block) {
			// current block is on the right, now check if it's the closest
			if(currentBlock < closestRightBlock) {
				closestRightBlock = currentBlock;
			}
		}
		currentBlock = currentBlock->nextBlock;
	}

	return closestRightBlock;
}

// checks if there's enough free space between blocks for allocation
static bool IsSpaceAvailable(const BlockHeader* a, const BlockHeader* b, uint16_t space) {
	assert(a <= b);

	// empty space must fit block header, required space and must be multiple of 4 bytes (pointer size)
	const uint32_t requiredSpace = sizeof(BlockHeader) + space + (space % sizeof(BlockHeader*));
	const uint32_t spaceBetween = (uint32_t)b - (uint32_t)a;

	assert(requiredSpace % sizeof(void*) == 0);
	assert(spaceBetween % sizeof(void*) == 0);

	if(spaceBetween >= requiredSpace) {
		return true;
	}
	return false;
}

static BlockAllocator GetNextFreeSpace(const BlockHeader* startBlock, uint16_t space) {
	BlockHeader* freeSpaceAddr = NULL;
	BlockHeader* currentBlock = startBlock;
	BlockHeader* lastBlock = NULL;	// last element in the allocation list

	if(currentBlock == NULL) {
		firstBlock = (BlockHeader*)&memory[0];
		memset(firstBlock, 0, sizeof(BlockHeader));
		freeSpaceAddr = firstBlock;
		lastBlock = firstBlock;
		currentBlock = NULL;
	}

	// iterate over all blocks until the space is found
	while(currentBlock != NULL) {
		const BlockHeader* closestRightBlock = GetClosestRightBlock(currentBlock);
		bool spaceAvailable = IsSpaceAvailable(currentBlock, closestRightBlock, space);
		if(spaceAvailable == true) {
			// free space found at the end of current block
			freeSpaceAddr = (BlockHeader*)((uint32_t)currentBlock + sizeof(BlockHeader) + currentBlock->blockSize);
			assert((uint32_t)freeSpaceAddr % sizeof(BlockHeader*) == 0);

			lastBlock = currentBlock;
			while(lastBlock->nextBlock != NULL) {
				lastBlock = lastBlock->nextBlock;
			}

			break;
		}
		currentBlock = currentBlock->nextBlock;
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
