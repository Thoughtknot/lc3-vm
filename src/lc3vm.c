#include "registers.h"
#include "memory.h"
#include "instructions.h"
#include "util.h"
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>

int read_image(Memory* mem, const char* image_path) {
    FILE* file = fopen(image_path, "rb");
    if (!file) 
        return 0;
    /* the origin tells us where in memory to place the image */
    uint16_t origin;
    fread(&origin, sizeof(origin), 1, file);
    origin = swap16(origin);

    /* we know the maximum file size so we only need one fread */
    uint16_t max_read = UINT16_MAX - origin;
    uint16_t* p = mem->memory + origin;
    size_t read = fread(p, sizeof(uint16_t), max_read, file);

    /* swap to little endian */
    while (read-- > 0) {
        *p = swap16(*p);
        ++p;
    }
    fclose(file);
    return 1;
}

/* terminal input setup */
struct termios original_tio;

void disable_input_buffering() {
  tcgetattr(STDIN_FILENO, &original_tio);
  struct termios new_tio = original_tio;
  new_tio.c_lflag &= ~ICANON & ~ECHO;
  tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void restore_input_buffering() {
  tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}

void handle_interrupt(int signal) {
  restore_input_buffering();
  printf("\n");
  exit(-2);
}

int main(int argc, const char* argv[]) {
    if (argc < 2)
    {
        /* show usage string */
        printf("lc3 [image-file1] ...\n");
        exit(2);
    }

    Memory* mem = create_memory();
    for (int j = 1; j < argc; ++j)
    {
        if (!read_image(mem, argv[j]))
        {
            printf("failed to load image: %s\n", argv[j]);
            exit(1);
        }
    }

    Register* reg = create_register();

    signal(SIGINT, handle_interrupt);
    disable_input_buffering();

    int running = 1;
    while (running)
    {
        /* FETCH */
        uint16_t instr = mem_read(mem, reg->reg[R_PC]++);
        //printf("instr %x, flag: %d\n", reg->reg[R_PC], reg->reg[R_COND]);
        running = handle_instruction(reg, mem, instr);
    }

    restore_input_buffering();
    
    free_memory(mem);
    free_register(reg);
}