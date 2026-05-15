#ifndef STITCH_BUFFER_ENGINE_H
#define STITCH_BUFFER_ENGINE_H

#include "../types.h"

void editorUpdateLine(Line *line);
void editorInsertLine(int at, char *s, size_t len);
void editorFreeLine(Line *line);
void editorDelLine(int at);
void editorFreeBuffer(void);

void editorInsertChar(int c);
void editorInsertNewline(void);
void editorDelChar(void);

#endif
