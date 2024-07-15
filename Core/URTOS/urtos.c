#include "urtos.h"
#include "Memory/urtos_memory.h"


void URTOS_Init(void) {

}

void* URTOS_Allocate(uint16_t bytesToAllocate) {
	return URTOS_Memory_Allocate(bytesToAllocate);
}
