#ifndef STITCH_TERMINAL_H
#define STITCH_TERMINAL_H

#include "stitch/types.h"

void core_die(const char *s);
void core_enable_raw_mode(StitchState *state);
void core_disable_raw_mode(void);
int core_get_window_size(int *rows, int *cols);
int core_read_key(StitchState *state);
int core_is_utf8_start(unsigned char c);

/* Compat / Shared Utils */
int editorIsUtf8Start(unsigned char c);
size_t editorRowByteToCol(const char *s, size_t len, size_t target_byte);
size_t editorRowColToByte(const char *s, size_t len, size_t target_col);

void *editorMalloc(size_t size);
void *editorRealloc(void *ptr, size_t size);
char *editorStrdup(const char *s);
char *editorStrcasestr(const char *haystack, const char *needle);

#endif
