#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <time.h>

// http://devernay.free.fr/hacks/chip8/C8TECH10.HTM

/**
    nnn or addr - A 12-bit value, the lowest 12 bits of the instruction
    n or nibble - A 4-bit value, the lowest 4 bits of the instruction
    x - A 4-bit value, the lower 4 bits of the high byte of the instruction
    y - A 4-bit value, the upper 4 bits of the low byte of the instruction
    kk or byte - An 8-bit value, the lowest 8 bits of the instruction
*/

#define BYTE_MSB(b)     ((b >> 7) & 1)
#define INSTRUCTION(i)  ((i >> 12) & 0x0f)
#define ADDRESS(i)      (i & 0x0fff)
#define NIBBLE(i)       (i & 0x000f)
#define	X(i)            ((i >> 8) & 0x0f)
#define Y(i)            ((i >> 4) & 0x0f)
#define KK(i)           (i & 0x00ff)
#define SWAP_INSTR(i)   ((i >> 8) | i << 8)	

#define RAM_SIZE        0x1000
#define ADDR_ZERO       0x000
#define FIRST_ADDR      0x002
#define LAST_ADDR       0xffe
#define ENTRY_POINT     0x200
#define MAX_ROM_SIZE    (RAM_SIZE - ENTRY_POINT)
#define SCREEN_WIDTH    64
#define SCREEN_HEIGHT   32
#define NS_60HZ         ((1 / 60) * 1000000000)

typedef struct registers {
    uint8_t v[0x10];    // general purpose registers
    uint16_t i;         // generally used to store memory addressess
    uint8_t dt;         // delay timer
    uint8_t st;         // sound timer
    uint16_t pc;        // program counter
    uint8_t sp;         // stack pointer - topmost level of the stack
} registers;

typedef struct chip8_ctx {
    registers regs;
    uint16_t stack[0x10];
    uint8_t ram[RAM_SIZE];
    uint8_t screen[SCREEN_HEIGHT][SCREEN_WIDTH]; // 64x32 pixel
    uint8_t fonts; // pointer to fonts in ram
    struct timespec timestamp;
} chip8_ctx;
 
#define SPRITE_SIZE     5
#define FONT_SPRITE_0   "\xf0\x90\x90\x90\xf0"
#define FONT_SPRITE_1   "\x20\x60\x20\x20\x70"
#define FONT_SPRITE_2   "\xf0\x10\xf0\x80\xf0"
#define FONT_SPRITE_3   "\xf0\x10\xf0\x10\xf0"
#define FONT_SPRITE_4   "\x90\x90\xf0\x10\x10"
#define FONT_SPRITE_5   "\xf0\x80\xf0\x10\xf0"
#define FONT_SPRITE_6   "\xf0\x80\xf0\x90\xf0"
#define FONT_SPRITE_7   "\xf0\x10\x20\x40\x40"
#define FONT_SPRITE_8   "\xf0\x90\xf0\x90\xf0"
#define FONT_SPRITE_9   "\xf0\x90\xf0\x10\xf0"
#define FONT_SPRITE_A   "\xf0\x90\xf0\x90\x90"
#define FONT_SPRITE_B   "\xe0\x90\xe0\x90\xe0"
#define FONT_SPRITE_C   "\xf0\x80\x80\x80\xf0"
#define FONT_SPRITE_D   "\xe0\x90\x90\x90\xe0"
#define FONT_SPRITE_E   "\xf0\x80\xf0\x80\xf0"
#define FONT_SPRITE_F   "\xf0\x80\xf0\x80\x80"

#define FONT_SPRITES FONT_SPRITE_0 FONT_SPRITE_1 FONT_SPRITE_2 FONT_SPRITE_3 \
                     FONT_SPRITE_4 FONT_SPRITE_5 FONT_SPRITE_6 FONT_SPRITE_7 \
                     FONT_SPRITE_8 FONT_SPRITE_9 FONT_SPRITE_A FONT_SPRITE_B \
                     FONT_SPRITE_C FONT_SPRITE_D FONT_SPRITE_E FONT_SPRITE_F

void chip8_init(chip8_ctx *ctx, const void *rom, int rom_size);
void chip8_exec(chip8_ctx *ctx);

#endif