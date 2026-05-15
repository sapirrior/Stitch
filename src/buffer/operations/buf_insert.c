#include <stdlib.h>
#include <string.h>
#include "stitch/types.h"
#include "stitch/core/terminal.h"
#include "buffer_internal.h"

void buffer_row_insert_char(StitchBuffer *buf, Line *line, int at, int c) {
    if (at < 0 || (size_t)at > line->size) at = (int)line->size;
    
    if (line->size + 1 >= line->capacity) {
        if (line->capacity == 0) line->capacity = 16;
        else line->capacity *= 2;
        line->chars = editorRealloc(line->chars, line->capacity);
    }
    
    memmove(&line->chars[at + 1], &line->chars[at], line->size - (size_t)at + 1);
    line->size++;
    line->chars[at] = c;
    buffer_update_line(line);
    buf->dirty++;
}

void buffer_insert_char(StitchBuffer *buf, StitchView *view, int c) {
    if ((size_t)view->cy == buf->num_lines) {
        buffer_insert_line(buf, buf->num_lines, "", 0);
    }
    buffer_row_insert_char(buf, &buf->lines[view->cy], view->cx, c);
    view->cx++;
}

void buffer_insert_newline(StitchBuffer *buf, StitchView *view) {
    if (view->cy < 0 || (size_t)view->cy >= buf->num_lines) {
        if ((size_t)view->cy == buf->num_lines) {
             buffer_insert_line(buf, buf->num_lines, "", 0);
        } else {
             return;
        }
    }

    if (view->cx == 0) {
        buffer_insert_line(buf, (size_t)view->cy, "", 0);
    } else {
        Line *line = &buf->lines[view->cy];
        buffer_insert_line(buf, (size_t)view->cy + 1, &line->chars[view->cx], line->size - (size_t)view->cx);
        line = &buf->lines[view->cy];
        line->size = (size_t)view->cx;
        line->chars[line->size] = '\0';
        buffer_update_line(line);
    }
    view->cy++;
    view->cx = 0;
}
