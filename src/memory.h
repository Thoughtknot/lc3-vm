#ifndef __memory
#define __memory
#include <stdint.h>
#include <stdlib.h>

enum memory_registers {
    MR_KBSR = 0xFE00, /* keyboard status */
    MR_KBDR = 0xFE02  /* keyboard data */
};

typedef struct Memory Memory;
struct Memory {
    uint16_t memory[UINT16_MAX];
};

Memory* create_memory();
void free_memory(Memory* mem);
uint16_t mem_read(Memory* mem, uint16_t address);
void mem_store(Memory* mem, uint16_t address, uint16_t value);
#endif