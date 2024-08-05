#include <stddef.h>
#include "urtos_memory_storage.h"


uint8_t memory[URTOS_CONFIG_MEMORY_ALLOCATED_SIZE];

BlockHeader* firstBlock = NULL;
