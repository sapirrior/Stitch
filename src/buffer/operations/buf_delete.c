#include <stdlib.h>
#include <string.h>
#include "stitch/types.h"
#include "stitch/core/terminal.h"
#include "buffer_internal.h"

void buffer_row_del_char(StitchBuffer *buf, Line *line, int at) {
    if (at < 0 || at >= line->size) return;
    memmove(&line->chars[at], &line->chars[at + 1], line->size - at);
    line->size--;
    buffer_update_line(line);
    buf->dirty++;
}

void buffer_del_char(StitchBuffer *buf, StitchView *view) {
    if (view->cy == buf->num_lines) return;
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
        view->cx = buf->lines[view->cy - 1].size;
        int prev_len = buf->lines[view->cy - 1].size;
        buf->lines[view->cy - 1].chars = editorRealloc(buf->lines[view->cy - 1].chars, prev_len + line->size + 1);
        memcpy(&buf->lines[view->cy - 1].chars[prev_len], line->chars, line->size);
        buf->lines[view->cy - 1].size += line->size;
        buf->lines[view->cy - 1].chars[buf->lines[view->cy - 1].size] = '\0';
        buffer_update_line(&buf->lines[view->cy - 1]);
        buffer_del_line(buf, view->cy);
        view->cy--;
    }
}
