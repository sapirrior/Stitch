#include <stdlib.h>
#include <string.h>
#include "stitch/types.h"
#include "stitch/core/terminal.h"
#include "buffer_internal.h"

void buffer_row_insert_char(StitchBuffer *buf, Line *line, int at, int c) {
    if (at < 0 || at > line->size) at = line->size;
    line->chars = editorRealloc(line->chars, line->size + 2);
    memmove(&line->chars[at + 1], &line->chars[at], line->size - at + 1);
    line->size++;
    line->chars[at] = c;
    buffer_update_line(line);
    buf->dirty++;
}

void buffer_insert_char(StitchBuffer *buf, StitchView *view, int c) {
    if (view->cy == buf->num_lines) {
        buffer_insert_line(buf, buf->num_lines, "", 0);
    }
    buffer_row_insert_char(buf, &buf->lines[view->cy], view->cx, c);
    view->cx++;
}

void buffer_insert_newline(StitchBuffer *buf, StitchView *view) {
    if (view->cx == 0) {
        buffer_insert_line(buf, view->cy, "", 0);
    } else {
        Line *line = &buf->lines[view->cy];
        buffer_insert_line(buf, view->cy + 1, &line->chars[view->cx], line->size - view->cx);
        line = &buf->lines[view->cy];
        line->size = view->cx;
        line->chars[line->size] = '\0';
        buffer_update_line(line);
    }
    view->cy++;
    view->cx = 0;
}
