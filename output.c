#include "output.h"

#include <stdio.h>
#include <stdlib.h>

void beep()
{
    printf("\a");
}

void chip8_print_screen(chip8_ctx *ctx)
{
    system("clear");
    for (int y = 0; y < 32; y++)
    {
        for (int x = 0; x < 64; x++)
        {
            printf(ctx->screen[y][x] ? "*" : " ");
        }
        printf("\n");
    }
}

void chip8_draw_screen(chip8_ctx *ctx, int px, int py, int n)
{
    for (int y = 0; y < n; y++)
    {
        char byte = ctx->ram[ctx->regs.i + y];
        for (int b = 0; b < 8; b++)
        {
            int bit = (byte >> (7 - b)) & 1;
            uint8_t *pixel = &ctx->screen[(py + y) % 32][(px + b) % 64];
            ctx->regs.v[0x0f] = *pixel && bit;
            *pixel = (*pixel ^ bit) & 1;
        }
    }
}