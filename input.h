#ifndef INPUT_H
#define INPUT_H

int readrom(const char *path, void *buffer, int size);
int kbhit(void);
int getch(void);
int getkey(void);

#endif