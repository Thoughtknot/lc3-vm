#ifndef __registers
#define __registers
#include <stdint.h>
#include <stdlib.h>

typedef struct Register Register;
enum registers
{
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC, /* program counter */
    R_COND,
    R_COUNT
};

enum conditions
{
    FL_POS = 1 << 0, /* P */
    FL_ZRO = 1 << 1, /* Z */
    FL_NEG = 1 << 2, /* N */
};

enum { PC_START = 0x3000 };

struct Register {
    uint16_t reg[R_COUNT];
};

Register* create_register();
void free_register(Register* reg);
void update_flags(Register* reg, uint16_t r);

#endif