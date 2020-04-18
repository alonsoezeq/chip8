# Chip-8 Interpreter

Chip-8 interpreter fast written while quarantining taking [Cowgod's Chip-8 Technical Reference v1.0](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM "Cowgod's Chip-8 Technical Reference") as reference.

## What is Chip-8?

As described in Cowgod's Technical Reference:

> Chip-8 is a simple, interpreted, programming language which was first used on some do-it-yourself computer systems in the late 1970s and early 1980s. The COSMAC VIP, DREAM 6800, and ETI 660 computers are a few examples. These computers typically were designed to use a television as a display, had between 1 and 4K of RAM, and used a 16-key hexadecimal keypad for input. The interpreter took up only 512 bytes of memory, and programs, which were entered into the computer in hexadecimal, were even smaller.

### Build

In linux:

```
$ make
```

### Usage

```
$ ./chip8
Usage: ./chip8 <rom-path>
```
