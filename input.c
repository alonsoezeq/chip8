#include "input.h"

#include <stdio.h>
// #include <conio.h> // Windows

// Linux
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

int readrom(const char *path, void *buffer, int size)
{
    int read = 0;
    FILE *file = fopen(path, "r");
    if (file != NULL)
    {
        read = fread(buffer, 1, size, file);
        fclose(file);
    }
    return read;
}

int kbhit(void)
{
    struct termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    
    struct termios newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
    int ch = getchar();
 
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
 
    if(ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

  return 0;
}

int getch(void)
{
    struct termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    struct termios newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    int ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

int getkey(void)
{
    int c = getch();
    if (c >= '0' && c <= '9') {c -= '0';}
    else if (c >= 'a' && c <= 'f') {c = (c - 'a') + 10;}
    else if (c >= 'A' && c <= 'F') {c = (c - 'A') + 10;}
    else {c = 0;}
    return c;
}