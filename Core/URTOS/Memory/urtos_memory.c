#include <stddef.h>
#include "urtos_memory.h"
#include "urtos_config.h"


typedef struct {
	uint16_t blockSize;
	uint16_t nextBlockIndex;
} URTOS_Memory_BlockHeader;


static uint16_t firstBlockIndex;

static uint8_t memory[URTOS_CONFIG_MEMORY_ALLOCATED_SIZE];


void* URTOS_Memory_Allocate(uint16_t bytesToAllocate) {
	if(bytesToAllocate == 0) {
		return NULL;
	}
}
