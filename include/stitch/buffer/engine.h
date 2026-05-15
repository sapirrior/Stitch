#ifndef STITCH_BUFFER_ENGINE_H
#define STITCH_BUFFER_ENGINE_H

#include "stitch/types.h"

void buffer_insert_line(StitchBuffer *buf, int at, char *s, size_t len);
void buffer_del_line(StitchBuffer *buf, int at);
void buffer_free(StitchBuffer *buf);

void buffer_insert_char(StitchBuffer *buf, StitchView *view, int c);
void buffer_insert_newline(StitchBuffer *buf, StitchView *view);
void buffer_del_char(StitchBuffer *buf, StitchView *view);

#endif
