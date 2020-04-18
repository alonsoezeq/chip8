#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "chip8.h"
#include "input.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage: %s <rom-path>\n", argv[0]);
        return 1;
    }

    int alloc_size = sizeof(chip8_ctx) + MAX_ROM_SIZE;
    uint8_t *alloc = (uint8_t *)malloc(alloc_size);
    if (alloc == NULL)
    {
        printf("Failed to allocate %i bytes\n", alloc_size);
        return 2;
    }

    chip8_ctx *ctx = (chip8_ctx *)alloc;
    uint8_t *rom = alloc + sizeof(chip8_ctx);

    int rom_size = readrom(argv[1], rom, MAX_ROM_SIZE);
    if (rom_size == 0)
    {
        printf("Can't read file %s\n", argv[1]);
        return 3;
    }

    chip8_init(ctx, rom, rom_size);

    for (; ; ) // maybe one day SCHIP will break the loop (00FD - EXIT instr)
    {
        chip8_exec(ctx);
        usleep(1 / 400); // Emulate 400Hz cycle
    }

    free(alloc); // <- (:

    return 0;
}
