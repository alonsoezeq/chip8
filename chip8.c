#include "chip8.h"

#include "input.h"
#include "output.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>

long ns_diff(struct timespec t0, struct timespec t1)
{
    struct timespec diff;
    if (t1.tv_nsec - t1.tv_nsec < 0)
    {
        diff.tv_sec = t1.tv_sec - t0.tv_sec - 1;
        diff.tv_nsec = t1.tv_nsec - t0.tv_nsec + 1000000000;
    }
    else
    {
        diff.tv_sec = t1.tv_sec - t0.tv_sec;
        diff.tv_nsec = t1.tv_nsec - t0.tv_nsec;
    }
    return (diff.tv_sec * 1000000000.0 + diff.tv_nsec);
}

void chip8_init(chip8_ctx *ctx, const void *rom, int rom_size)
{
    char fonts[] = FONT_SPRITES;
    srand(time(NULL));   

    memset(ctx, 0, sizeof(chip8_ctx));

    ctx->fonts = FIRST_ADDR;
    memcpy(ctx->ram + ctx->fonts, fonts, sizeof(fonts) - 1);
    
    ctx->regs.pc = ENTRY_POINT;
    memcpy(ctx->ram + ctx->regs.pc, rom, rom_size);

    clock_gettime(CLOCK_REALTIME, &ctx->timestamp);
}

void chip8_exec(chip8_ctx *ctx)
{
    uint16_t instr = SWAP_INSTR(*(uint16_t *)(ctx->ram + ctx->regs.pc));
    ctx->regs.pc += sizeof(instr);

    switch (INSTRUCTION(instr))
    {
        case 0x00:
            switch (KK(instr))
            {
                case 0xe0: // CLS - Clear the display.
                    memset(ctx->screen, 0, sizeof(ctx->screen));
                    chip8_print_screen(ctx);
                    break;
                case 0xee: // RET - Return from a subroutine.
                    ctx->regs.pc = ctx->stack[ctx->regs.sp];
                    ctx->regs.sp--;
                    break;
                default:   // SYS addr - Jump to a machine code routine at nnn. Ignored.
                    break;
            }
            break;
        case 0x01: // JP addr - Jump to location nnn.
            ctx->regs.pc = ADDRESS(instr);
            break;
        case 0x02: // CALL addr - Call subroutine at nnn.
            ctx->regs.sp++;
            ctx->stack[ctx->regs.sp] = (ctx->regs.pc);
            ctx->regs.pc = ADDRESS(instr);
            break;
        case 0x03: // SE Vx, byte - Skip next instruction if Vx = kk.
            ctx->regs.pc += (ctx->regs.v[X(instr)] == KK(instr)) * sizeof(instr);
            break;
        case 0x04: // SNE Vx, byte - Skip next instruction if Vx != kk.
            ctx->regs.pc += (ctx->regs.v[X(instr)] != KK(instr)) * sizeof(instr);
            break;
        case 0x05:
            switch (NIBBLE(instr))
            {
                case 0x00: // SE Vx, Vy - Skip next instruction if Vx = Vy.
                    ctx->regs.pc += (ctx->regs.v[X(instr)] == ctx->regs.v[Y(instr)]) * sizeof(instr);
                    break;
                default:
                    break;
            }
            break;
        case 0x06: // LD Vx, byte - Set Vx = kk.
            ctx->regs.v[X(instr)] = KK(instr);
            break;
        case 0x07: // ADD Vx, byte - Set Vx = Vx + kk.
            ctx->regs.v[X(instr)] += KK(instr);
            break;
        case 0x08:
            switch (NIBBLE(instr))
            {
                case 0x00: // LD Vx, Vy - Set Vx = Vy.
                    ctx->regs.v[X(instr)] = ctx->regs.v[Y(instr)];
                    break;
                case 0x01: // OR Vx, Vy - Set Vx = Vx OR Vy.
                    ctx->regs.v[X(instr)] |= ctx->regs.v[Y(instr)];
                    break;
                case 0x02: // AND Vx, Vy - Set Vx = Vx AND Vy.
                    ctx->regs.v[X(instr)] &= ctx->regs.v[Y(instr)];
                    break;
                case 0x03: // XOR Vx, Vy - Set Vx = Vx XOR Vy.
                    ctx->regs.v[X(instr)] ^= ctx->regs.v[Y(instr)];
                    break;
                case 0x04: // ADD Vx, Vy - Set Vx = Vx + Vy, set VF = carry.
                    ctx->regs.v[X(instr)] += ctx->regs.v[Y(instr)];
                    ctx->regs.v[0x0f] = (ctx->regs.v[X(instr)] < ctx->regs.v[Y(instr)]); 
                    break;
                case 0x05: // SUB Vx, Vy - Set Vx = Vx - Vy, set VF = NOT borrow.
                    ctx->regs.v[0x0f] = (ctx->regs.v[X(instr)] > ctx->regs.v[Y(instr)]);
                    ctx->regs.v[X(instr)] -= ctx->regs.v[Y(instr)];
                    break;
                case 0x06: // SHR Vx {, Vy} - Set Vx = Vx SHR 1.
                    ctx->regs.v[0x0f] = ctx->regs.v[X(instr)] & 1;
                    ctx->regs.v[X(instr)] >>= 1;
                    break;
                case 0x07: // SUBN Vx, Vy - Set Vx = Vy - Vx, set VF = NOT borrow.
                    ctx->regs.v[0x0f] = (ctx->regs.v[Y(instr)] > ctx->regs.v[X(instr)]);
                    ctx->regs.v[X(instr)] = ctx->regs.v[Y(instr)] - ctx->regs.v[X(instr)];
                    break;
                case 0x0e: // SHL Vx {, Vy} - Set Vx = Vx SHL 1.
                    ctx->regs.v[0x0f] = BYTE_MSB(ctx->regs.v[X(instr)]);
                    ctx->regs.v[X(instr)] <<= 1;
                    break;
                default:
                    break;
            }
            break;
        case 0x09: // SNE Vx, Vy - Skip next instruction if Vx != Vy.
            ctx->regs.pc += (ctx->regs.v[X(instr)] != ctx->regs.v[Y(instr)]) * sizeof(instr);
            break;
        case 0x0a: // LD I, addr - Set I = nnn.
            ctx->regs.i = ADDRESS(instr);
            break;
        case 0x0b: // JP V0, addr - Jump to location nnn + V0.
            ctx->regs.pc = ADDRESS(instr) + ctx->regs.v[0];
            break;
        case 0x0c: // RND Vx, byte - Set Vx = random byte AND kk.
            ctx->regs.v[X(instr)] = (rand() % 0x100) & KK(instr);
            break;
        case 0x0d: // DRW Vx, Vy, nibble - Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
            chip8_draw_screen(ctx, ctx->regs.v[X(instr)], ctx->regs.v[Y(instr)], NIBBLE(instr));
            chip8_print_screen(ctx);
            break;
        case 0x0e:
            switch (KK(instr))
            {
                case 0x9e: // SKP Vx - Skip next instruction if key with the value of Vx is pressed.
                    if (kbhit() && X(instr) == getkey()) {
                        ctx->regs.pc += sizeof(instr);
                    }
                    break;
                case 0xa1: // SKNP Vx - Skip next instruction if key with the value of Vx is not pressed.
                    if (kbhit() && X(instr) != getkey()) {
                        ctx->regs.pc += sizeof(instr);
                    }
                    break;
                default:
                    break;
            }
            break;
        case 0x0f:
            switch (KK(instr))
            {
                case 0x07: // LD Vx, DT - Set Vx = delay timer value.
                    ctx->regs.v[X(instr)] = ctx->regs.dt;
                    break;
                case 0x0a: // LD Vx, K - Wait for a key press, store the value of the key in Vx.
                    ctx->regs.v[X(instr)] = getkey();
                    break;
                case 0x15: // LD DT, Vx - Set delay timer = Vx.
                    ctx->regs.dt = ctx->regs.v[X(instr)];
                    break;
                case 0x18: // LD ST, Vx - Set sound timer = Vx.
                    ctx->regs.st = ctx->regs.v[X(instr)];
                    break;
                case 0x1e: // ADD I, Vx - Set I = I + Vx.
                    ctx->regs.i += ctx->regs.v[X(instr)];
                    break;
                case 0x29: // LD F, Vx - Set I = location of sprite for digit Vx.
                    ctx->regs.i = ctx->fonts + ctx->regs.v[X(instr)] * SPRITE_SIZE;
                    break;
                case 0x33: // LD B, Vx - Store BCD representation of Vx in memory locations I, I+1, and I+2.
                    for (int i = 2, c = ctx->regs.v[X(instr)]; i >= 0 && c > 0; i--, c /= 10)
                    {
                        ctx->ram[ctx->regs.i + i] = c % 10;
                    }
                    break;
                case 0x55: // LD [I], Vx - Store registers V0 through Vx in memory starting at location I
                    for (int i = 0; i <= X(instr); i++)
                    {
                        ctx->ram[ctx->regs.i + i] = ctx->regs.v[i];
                    }
                    break;
                case 0x65: // LD Vx, [I] - Read registers V0 through Vx from memory starting at location I.
                    for (int i = 0; i <= X(instr); i++)
                    {
                        ctx->regs.v[i] = ctx->ram[ctx->regs.i + i];
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    struct timespec t0;
    t0.tv_nsec = ctx->timestamp.tv_nsec;
    t0.tv_sec = ctx->timestamp.tv_sec;

    clock_gettime(CLOCK_REALTIME, &ctx->timestamp);
    
    if (ns_diff(t0, ctx->timestamp) > (1 / 60) * 1000000000) // 60Hz
    {
        if (ctx->regs.dt > 0) {ctx->regs.dt--;}
        if (ctx->regs.st > 0) {ctx->regs.st--;} // ToDo: beep while > 0
    }
}