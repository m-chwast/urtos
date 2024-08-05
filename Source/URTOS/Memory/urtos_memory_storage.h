#ifndef URTOS_MEMORY_URTOS_MEMORY_STORAGE_H_
#define URTOS_MEMORY_URTOS_MEMORY_STORAGE_H_

#include <stdint.h>
#include "../urtos_config.h"


typedef struct URTOS_Memory_BlockHeader {
	uint32_t blockSize;
	struct URTOS_Memory_BlockHeader* nextBlock;
} BlockHeader;


extern uint8_t memory[URTOS_CONFIG_MEMORY_ALLOCATED_SIZE];

extern BlockHeader* firstBlock;


#endif /* URTOS_MEMORY_URTOS_MEMORY_STORAGE_H_ */
