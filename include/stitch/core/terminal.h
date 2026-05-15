#ifndef STITCH_TERMINAL_H
#define STITCH_TERMINAL_H

#include "../types.h"

void die(const char *s);
void enableRawMode(void);
void disableRawMode(void);
int getWindowSize(int *rows, int *cols);
int editorReadKey(void);

/* Robust I/O */
void writeAll(int fd, const char *buf, size_t len);

/* UTF-8 Utilities */
int editorIsUtf8Start(unsigned char c);
int editorRowByteToCol(const char *s, int len, int target_byte);
int editorRowColToByte(const char *s, int len, int target_col);

void *editorMalloc(size_t size);
void *editorRealloc(void *ptr, size_t size);
char *editorStrdup(const char *s);
char *editorStrcasestr(const char *haystack, const char *needle);

#endif
