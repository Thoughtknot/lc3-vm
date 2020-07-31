#include "instructions.h"
#include <stdio.h>

void load_indirect(Register* reg, Memory* mem, uint16_t instr) {
    /* destination register (DR) */
    uint16_t r0 = (instr >> 9) & 0b111;
    uint16_t pcoffset9 = sign_extend(instr & 0b111111111, 9);
    reg->reg[r0] = mem_read(mem, mem_read(mem, reg->reg[R_PC] + pcoffset9));
    update_flags(reg, r0);
}

void branch(Register* reg, Memory* mem, uint16_t instr) {
    uint16_t n_flag = (instr >> 11) & 0x1;
    uint16_t z_flag = (instr >> 10) & 0x1;
    uint16_t p_flag = (instr >> 9) & 0x1;
    uint16_t pcoffset9 = sign_extend(instr & 0x1FF, 9);
    uint16_t cond = reg->reg[R_COND];
    if ((n_flag && (cond & FL_NEG)) ||
            (z_flag && (cond & FL_ZRO)) ||
            (p_flag && (cond & FL_POS)))
        reg->reg[R_PC] += pcoffset9;
}

void jump(Register* reg, uint16_t instr) {
    uint16_t r1 = (instr >> 6) & 0b111;
    reg->reg[R_PC] = reg->reg[r1];
}

void jump_to_sr(Register* reg, uint16_t instr) {
    reg->reg[R_R7] = reg->reg[R_PC];
    if ((instr >> 11) & 0b1) {
        uint16_t pcoffset11 = sign_extend(instr & 0b11111111111, 11);
        reg->reg[R_PC] = reg->reg[R_PC] + pcoffset11;
    }
    else {
        int baseR = (instr >> 6) & 0b111;
        reg->reg[R_PC] = reg->reg[baseR];
    }
}

void add(Register* reg, uint16_t instr) {
    /* destination register (DR) */
    uint16_t r0 = (instr >> 9) & 0b111;
    /* first operand (SR1) */
    uint16_t r1 = (instr >> 6) & 0b111;
    /* whether we are in immediate mode */
    uint16_t imm_flag = (instr >> 5) & 0b1;
    if (imm_flag) {
        uint16_t val = sign_extend(instr & 0x1F, 5);
        reg->reg[r0] = reg->reg[r1] + val;
    }
    else {
        uint16_t val = reg->reg[instr & 0b111];
        reg->reg[r0] = reg->reg[r1] + val;
    }
    update_flags(reg, r0);
}

void and(Register* reg, uint16_t instr) {
    /* destination register (DR) */
    uint16_t r0 = (instr >> 9) & 0b111;
    /* first operand (SR1) */
    uint16_t r1 = (instr >> 6) & 0b111;
    /* whether we are in immediate mode */
    uint16_t imm_flag = (instr >> 5) & 0b1;
    if (imm_flag) {
        uint16_t val = sign_extend(instr & 0x1F, 5);
        reg->reg[r0] = reg->reg[r1] & val;
    }
    else {
        uint16_t val = reg->reg[instr & 0b111];
        reg->reg[r0] = reg->reg[r1] & val;
    }
    update_flags(reg, r0);
}

void not(Register* reg, uint16_t instr) {
    /* destination register (DR) */
    uint16_t r0 = (instr >> 9) & 0b111;
    uint16_t r1 = (instr >> 6) & 0b111;
    reg->reg[r0] = ~reg->reg[r1];
    update_flags(reg, r0);
}

void load(Register* reg, Memory* mem, uint16_t instr) {
    /* destination register (DR) */
    uint16_t r0 = (instr >> 9) & 0b111;
    uint16_t pcoffset9 = sign_extend(instr & 0b111111111, 9);
    reg->reg[r0] = mem_read(mem, reg->reg[R_PC] + pcoffset9);
    update_flags(reg, r0);
}

void load_base_offset(Register* reg, Memory* mem, uint16_t instr) {
    /* destination register (DR) */
    uint16_t r0 = (instr >> 9) & 0b111;
    uint16_t r1 = (instr >> 6) & 0b111;
    uint16_t pcoffset6 = sign_extend(instr & 0b111111, 6);
    reg->reg[r0] = mem_read(mem, reg->reg[r1] + pcoffset6);
    update_flags(reg, r0);
}

void load_effective_address(Register* reg, Memory* mem, uint16_t instr) {
    /* destination register (DR) */
    uint16_t r0 = (instr >> 9) & 0b111;
    uint16_t pcoffset9 = sign_extend(instr & 0b111111111, 9);
    reg->reg[r0] = reg->reg[R_PC] + pcoffset9;
    update_flags(reg, r0);
}

void store(Register* reg, Memory* mem, uint16_t instr) {
    uint16_t sr = (instr >> 9) & 0b111;
    uint16_t pcoffset9 = sign_extend(instr & 0x1FF, 9);
    mem_store(mem, reg->reg[R_PC] + pcoffset9, reg->reg[sr]);
}

void store_indirect(Register* reg, Memory* mem, uint16_t instr) {
    uint16_t sr = (instr >> 9) & 0b111;
    uint16_t pcoffset9 = sign_extend(instr & 0x1FF, 9);
    uint16_t address = mem_read(mem, reg->reg[R_PC] + pcoffset9);
    mem_store(mem, address, reg->reg[sr]);
}

void store_base_offset(Register* reg, Memory* mem, uint16_t instr) {
    uint16_t sr = (instr >> 9) & 0b111;
    uint16_t baser = (instr >> 6) & 0b111;
    uint16_t pcoffset6 = sign_extend(instr & 0x3F, 6);
    mem_store(mem, reg->reg[baser] + pcoffset6, reg->reg[sr]);
}

enum trap {
    TRAP_GETC = 0x20,  /* get character from keyboard, not echoed onto the terminal */
    TRAP_OUT = 0x21,   /* output a character */
    TRAP_PUTS = 0x22,  /* output a word string */
    TRAP_IN = 0x23,    /* get character from keyboard, echoed onto the terminal */
    TRAP_PUTSP = 0x24, /* output a byte string */
    TRAP_HALT = 0x25   /* halt the program */
};

int trap(Register* reg, Memory* mem, uint16_t instr) {
    reg->reg[R_R7] = reg->reg[R_PC];
    switch (instr & 0xFF) {
        case TRAP_GETC:{
            char ch = getc(stdin);
            reg->reg[R_R0] = ch;
        }
        break;
        case TRAP_OUT: {
            char ch = (char) reg->reg[R_R0];
            putc(ch, stdout);
            fflush(stdout);
        }
        break;
        case TRAP_PUTS: {
            uint16_t* val = mem->memory + reg->reg[R_R0];
            while (*val) {
                putc((char) (*val & 0xFF), stdout);
                ++val;
            }
            fflush(stdout); 
        }
        break;
        case TRAP_IN:
        {
            printf("Enter a character: ");
            fflush(stdout);

            char c = getc(stdin);
            putc(c, stdout);
            fflush(stdout);
            reg->reg[R_R0] = (uint16_t) c;
        }
        break;
        case TRAP_PUTSP: {
            uint16_t* value = mem->memory + reg->reg[R_R0];
            while (*value) {
                char c1 = (*value) & 0xFF;
                putc(c1, stdout);
                char c2 = (*value) >> 8;
                if (c2)
                    putc(c2, stdout);
                value++;
            }
            fflush(stdout);
        }
        break;
        case TRAP_HALT:
        return 0;
    }
    return 1;
}

int handle_instruction(Register* reg, Memory* mem, uint16_t instr) {
    uint16_t opcode = instr >> 12;
    switch (opcode) {
        case OP_ADD:
            add(reg, instr);
            break;
        case OP_AND:
            and(reg, instr);
            break;
        case OP_NOT:
            not(reg, instr);
            break;
        case OP_BR:
            branch(reg, mem, instr);
            break;
        case OP_JMP:
            jump(reg, instr);
            break;
        case OP_JSR:
            jump_to_sr(reg, instr);
            break;
        case OP_LD:
            load(reg, mem, instr);
            break;
        case OP_LDI:
            load_indirect(reg, mem, instr);
            break;
        case OP_LDR:
            load_base_offset(reg, mem, instr);
            break;
        case OP_LEA:
            load_effective_address(reg, mem, instr);
            break;
        case OP_ST:
            store(reg, mem, instr);
            break;
        case OP_STI:
            store_indirect(reg, mem, instr);
            break;
        case OP_STR:
            store_base_offset(reg, mem, instr);
            break;
        case OP_TRAP:
            return trap(reg, mem, instr);
        case OP_RES:
        case OP_RTI:
        default:
            abort();
            break;
    }
    return 1;
}