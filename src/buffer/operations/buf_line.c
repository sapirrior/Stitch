#include <stdlib.h>
#include <string.h>
#include "stitch/types.h"
#include "stitch/core/terminal.h"
#include "buffer_internal.h"

void buffer_update_line(Line *line) {
    int tabs = 0;
    int nonprint = 0;
    for (int j = 0; j < line->size; j++) {
        unsigned char c = (unsigned char)line->chars[j];
        if (c == '\t') tabs++;
        else if (c < 32 || c == 127) nonprint++;
    }

    free(line->render);
    line->render = editorMalloc(line->size + tabs * (STITCH_TAB_STOP - 1) + nonprint + 1);

    int idx = 0;
    for (int j = 0; j < line->size; j++) {
        unsigned char c = (unsigned char)line->chars[j];
        if (c == '\t') {
            line->render[idx++] = ' ';
            while (idx % STITCH_TAB_STOP != 0) line->render[idx++] = ' ';
        } else if (c < 32 || c == 127) {
            line->render[idx++] = '^';
            line->render[idx++] = (c <= 26) ? '@' + c : '?';
        } else {
            line->render[idx++] = c;
        }
    }
    line->render[idx] = '\0';
    line->rsize = idx;
}

void buffer_insert_line(StitchBuffer *buf, int at, char *s, size_t len) {
    if (at < 0 || at > buf->num_lines) return;

    buf->lines = editorRealloc(buf->lines, sizeof(Line) * (buf->num_lines + 1));
    memmove(&buf->lines[at + 1], &buf->lines[at], sizeof(Line) * (buf->num_lines - at));

    buf->lines[at].size = (int)len;
    buf->lines[at].chars = editorMalloc(len + 1);
    memcpy(buf->lines[at].chars, s, len);
    buf->lines[at].chars[len] = '\0';

    buf->lines[at].rsize = 0;
    buf->lines[at].render = NULL;
    buffer_update_line(&buf->lines[at]);

    buf->num_lines++;
    buf->dirty++;
}

void buffer_free_line(Line *line) {
    free(line->chars);
    free(line->render);
}

void buffer_del_line(StitchBuffer *buf, int at) {
    if (at < 0 || at >= buf->num_lines) return;
    buffer_free_line(&buf->lines[at]);
    memmove(&buf->lines[at], &buf->lines[at + 1], sizeof(Line) * (buf->num_lines - at - 1));
    buf->num_lines--;
    buf->dirty++;
}
