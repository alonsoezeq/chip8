CC=gcc
CFLAGS=-Wall -Wextra
OUTPUT=chip8

build:
	$(CC) $(CFLAGS) main.c chip8.c input.c output.c -o $(OUTPUT)

clean:
	rm -f $(OUTPUT)
