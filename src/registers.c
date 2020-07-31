#include "registers.h"

Register* create_register() {
    Register* reg = (Register*) malloc(sizeof(Register));
    reg->reg[R_PC] = PC_START;
    return reg;
}

void free_register(Register* reg) {
    free(reg);
}

void update_flags(Register* reg, uint16_t r) {
    if (reg->reg[r] == 0)
    {
        reg->reg[R_COND] = FL_ZRO;
    }
    else if (reg->reg[r] >> 15) /* a 1 in the left-most bit indicates negative */
    {
        reg->reg[R_COND] = FL_NEG;
    }
    else
    {
        reg->reg[R_COND] = FL_POS;
    }
}