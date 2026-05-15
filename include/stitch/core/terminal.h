#ifndef STITCH_TERMINAL_H
#define STITCH_TERMINAL_H

#include "../types.h"

void die(const char *s);
void enableRawMode(void);
void disableRawMode(void);
int getWindowSize(int *rows, int *cols);
int editorReadKey(void);
void handleSigwinch(int sig);

#endif
