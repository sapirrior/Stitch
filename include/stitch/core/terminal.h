#ifndef STITCH_TERMINAL_H
#define STITCH_TERMINAL_H

#include "../types.h"

void die(const char *s);
void enableRawMode(void);
void disableRawMode(void);
int getWindowSize(int *rows, int *cols);
int editorReadKey(void);
void setupSignals(void);
void handleSigwinch(int sig);

void *editorMalloc(size_t size);
void *editorRealloc(void *ptr, size_t size);
char *editorStrdup(const char *s);
char *editorStrcasestr(const char *haystack, const char *needle);

#endif
