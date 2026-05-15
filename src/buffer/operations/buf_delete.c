#include <stdlib.h>
#include <string.h>
#include "stitch/types.h"
#include "stitch/core/terminal.h"
#include "buffer_internal.h"

void buffer_row_del_char(StitchBuffer *buf, Line *line, int at) {
    if (at < 0 || (size_t)at >= line->size) return;
    memmove(&line->chars[at], &line->chars[at + 1], line->size - (size_t)at);
    line->size--;
    buffer_update_line(line);
    buf->dirty++;
}

void buffer_del_char(StitchBuffer *buf, StitchView *view) {
    if ((size_t)view->cy == buf->num_lines) return;
    if (view->cx == 0 && view->cy == 0) return;

    Line *line = &buf->lines[view->cy];
    if (view->cx > 0) {
        int bytes_to_del = 1;
        while (view->cx - bytes_to_del > 0 && (line->chars[view->cx - bytes_to_del] & 0xc0) == 0x80) {
            bytes_to_del++;
        }
        while (bytes_to_del--) {
            buffer_row_del_char(buf, line, view->cx - 1);
            view->cx--;
        }
    } else {
        size_t prev_line_idx = (size_t)view->cy - 1;
        view->cx = (int)buf->lines[prev_line_idx].size;
        size_t prev_len = buf->lines[prev_line_idx].size;
        size_t needed = prev_len + line->size + 1;
        
        if (needed > buf->lines[prev_line_idx].capacity) {
            buf->lines[prev_line_idx].capacity = needed;
            buf->lines[prev_line_idx].chars = editorRealloc(buf->lines[prev_line_idx].chars, buf->lines[prev_line_idx].capacity);
        }
        
        memcpy(&buf->lines[prev_line_idx].chars[prev_len], line->chars, line->size);
        buf->lines[prev_line_idx].size += line->size;
        buf->lines[prev_line_idx].chars[buf->lines[prev_line_idx].size] = '\0';
        buffer_update_line(&buf->lines[prev_line_idx]);
        buffer_del_line(buf, (size_t)view->cy);
        view->cy--;
    }
}
