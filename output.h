#ifndef OUTPUT_H
#define OUTPUT_H

#include "chip8.h"

void chip8_print_screen(chip8_ctx *ctx);
void chip8_draw_screen(chip8_ctx *ctx, int px, int py, int n);

#endif