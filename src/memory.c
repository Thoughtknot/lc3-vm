#include "memory.h"
/* unix */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

uint16_t check_key() {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return select(1, &readfds, NULL, NULL, &timeout) != 0;
}

uint16_t mem_read(Memory* mem, uint16_t address) {
    if (address == MR_KBSR) {
        if (check_key())
        {
            mem->memory[MR_KBSR] = (1 << 15);
            mem->memory[MR_KBDR] = getchar();
        }
        else
        {
            mem->memory[MR_KBSR] = 0;
        }
    }
    return mem->memory[address];
}

void mem_store(Memory* mem, uint16_t address, uint16_t value) {
    mem->memory[address] = value;
}

Memory* create_memory() {
    Memory* mem = (Memory*) malloc(sizeof(Memory));
    return mem;
}

void free_memory(Memory* mem) {
    free(mem);
}