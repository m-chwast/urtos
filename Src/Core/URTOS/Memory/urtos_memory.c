#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "urtos_memory.h"
#include "urtos_config.h"

typedef struct URTOS_Memory_BlockHeader {
	uint32_t blockSize;
	struct URTOS_Memory_BlockHeader* nextBlock;
} BlockHeader;


static BlockHeader* firstBlock = NULL;

static uint8_t memory[URTOS_CONFIG_MEMORY_ALLOCATED_SIZE];

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

	const BlockHeader* closestRightBlock = (const BlockHeader*)&memory[URTOS_CONFIG_MEMORY_ALLOCATED_SIZE];

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

static bool IsSpaceAvailableBetweenAddresses(uint32_t a, uint32_t b, uint32_t space) {
	assert(a <= b);
	assert(space % sizeof(void*) == 0);

	const uint32_t requiredSpace = sizeof(BlockHeader) + space;
	const uint32_t spaceBetween = b - a;

	assert(requiredSpace % sizeof(void*) == 0);
	assert(spaceBetween % sizeof(void*) == 0);

	if(spaceBetween >= requiredSpace) {
		return true;
	}
	return false;
}

// checks if there's enough free space between blocks for allocation
static bool IsSpaceAvailable(const BlockHeader* a, const BlockHeader* b, uint32_t space) {
	assert(a <= b);

	const void* firstUnallocatedMemory = (void*)((uint32_t)a + sizeof(*a) + a->blockSize);

	bool isSpaceAvailable = IsSpaceAvailableBetweenAddresses((uint32_t)firstUnallocatedMemory, (uint32_t)b, space);
	return isSpaceAvailable;
}

static BlockHeader* TryFindFreeSpaceOnLeftBorder(uint32_t space) {
	const BlockHeader* currentBlock = firstBlock;
	const BlockHeader* leftmostBlock = currentBlock;
	while(currentBlock != NULL) {
		if(currentBlock < leftmostBlock) {
			leftmostBlock = currentBlock;
		}
		currentBlock = currentBlock->nextBlock;
	}

	BlockHeader* leftMemoryBorder = (BlockHeader*)&memory[0];
	bool spaceAvailable = IsSpaceAvailableBetweenAddresses((uint32_t)leftMemoryBorder, (uint32_t)leftmostBlock, space);

	if(spaceAvailable == false) {
		return NULL;
	}
	return leftMemoryBorder;
}

static BlockHeader* GetNextFreeSpace(const BlockHeader* startBlock, uint32_t space) {
	BlockHeader* freeSpaceAddr = NULL;
	const BlockHeader* currentBlock = startBlock;

	if(currentBlock == NULL) {
		firstBlock = (BlockHeader*)&memory[0];
		memset(firstBlock, 0, sizeof(BlockHeader));
		freeSpaceAddr = firstBlock;
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
			break;
		}
		currentBlock = currentBlock->nextBlock;
	}

	if(freeSpaceAddr == NULL) {
		freeSpaceAddr = TryFindFreeSpaceOnLeftBorder(space);
	}

	return freeSpaceAddr;
}

void* URTOS_Memory_Allocate(uint32_t bytesToAllocate) {
	if(bytesToAllocate == 0) {
		return NULL;
	}
	if(bytesToAllocate % sizeof(void*)) {
		bytesToAllocate += sizeof(void*) - (bytesToAllocate % sizeof(void*));
	}

	BlockHeader* freeSpaceBlock = GetNextFreeSpace(firstBlock, bytesToAllocate);

	// allocation failure
	if(freeSpaceBlock == NULL) {
		return NULL;
	}

	BlockHeader* lastBlock = firstBlock;
	while(lastBlock->nextBlock != NULL) {
		lastBlock = lastBlock->nextBlock;
	}

	if(lastBlock != freeSpaceBlock) {
		// check needed for first allocation
		lastBlock->nextBlock = freeSpaceBlock;
	}

	freeSpaceBlock->blockSize = bytesToAllocate;

	void* result = freeSpaceBlock + 1;
	return result;
}
