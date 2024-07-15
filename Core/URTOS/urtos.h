#ifndef URTOS_H_
#define URTOS_H_

#include <stdint.h>


void URTOS_Init(void);

void* URTOS_Allocate(uint16_t bytesToAllocate);


#endif
