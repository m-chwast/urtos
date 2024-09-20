#ifndef URTOS_MEMORY_H_
#define URTOS_MEMORY_H_

#include <stdint.h>


void* URTOS_Memory_Allocate(uint32_t bytesToAllocate);

void URTOS_Memory_Free(void* allocatedBlock);


#endif
