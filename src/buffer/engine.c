#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stitch/buffer/engine.h"
#include "stitch/core/terminal.h"

void editorUpdateLine(Line *line) {
    int tabs = 0;
    for (int j = 0; j < line->size; j++)
        if (line->chars[j] == '\t') tabs++;

    free(line->render);
    line->render = editorMalloc(line->size + tabs * (STITCH_TAB_STOP - 1) + 1);

    int idx = 0;
    for (int j = 0; j < line->size; j++) {
        if (line->chars[j] == '\t') {
            line->render[idx++] = ' ';
            while (idx % STITCH_TAB_STOP != 0) line->render[idx++] = ' ';
        } else {
            line->render[idx++] = line->chars[j];
        }
    }
    line->render[idx] = '\0';
    line->rsize = idx;
}

void editorInsertLine(int at, char *s, size_t len) {
    if (at < 0 || at > E.num_lines) return;

    E.lines = editorRealloc(E.lines, sizeof(Line) * (E.num_lines + 1));
    memmove(&E.lines[at + 1], &E.lines[at], sizeof(Line) * (E.num_lines - at));

    E.lines[at].size = (int)len;
    E.lines[at].chars = editorMalloc(len + 1);
    memcpy(E.lines[at].chars, s, len);
    E.lines[at].chars[len] = '\0';

    E.lines[at].rsize = 0;
    E.lines[at].render = NULL;
    editorUpdateLine(&E.lines[at]);

    E.num_lines++;
    E.dirty++;
}

void editorFreeLine(Line *line) {
    free(line->chars);
    free(line->render);
}

void editorDelLine(int at) {
    if (at < 0 || at >= E.num_lines) return;
    editorFreeLine(&E.lines[at]);
    memmove(&E.lines[at], &E.lines[at + 1], sizeof(Line) * (E.num_lines - at - 1));
    E.num_lines--;
    E.dirty++;
}

void editorRowInsertChar(Line *line, int at, int c) {
    if (at < 0 || at > line->size) at = line->size;
    line->chars = editorRealloc(line->chars, line->size + 2);
    memmove(&line->chars[at + 1], &line->chars[at], line->size - at + 1);
    line->size++;
    line->chars[at] = c;
    editorUpdateLine(line);
    E.dirty++;
}

void editorRowDelChar(Line *line, int at) {
    if (at < 0 || at >= line->size) return;
    memmove(&line->chars[at], &line->chars[at + 1], line->size - at);
    line->size--;
    editorUpdateLine(line);
    E.dirty++;
}

void editorInsertChar(int c) {
    if (E.cy == E.num_lines) {
        editorInsertLine(E.num_lines, "", 0);
    }
    editorRowInsertChar(&E.lines[E.cy], E.cx, c);
    E.cx++;
}

void editorInsertNewline(void) {
    if (E.cx == 0) {
        editorInsertLine(E.cy, "", 0);
    } else {
        Line *line = &E.lines[E.cy];
        editorInsertLine(E.cy + 1, &line->chars[E.cx], line->size - E.cx);
        line = &E.lines[E.cy];
        line->size = E.cx;
        line->chars[line->size] = '\0';
        editorUpdateLine(line);
    }
    E.cy++;
    E.cx = 0;
}

void editorDelChar(void) {
    if (E.cy == E.num_lines) return;
    if (E.cx == 0 && E.cy == 0) return;

    Line *line = &E.lines[E.cy];
    if (E.cx > 0) {
        editorRowDelChar(line, E.cx - 1);
        E.cx--;
    } else {
        E.cx = E.lines[E.cy - 1].size;
        int prev_len = E.lines[E.cy - 1].size;
        E.lines[E.cy - 1].chars = editorRealloc(E.lines[E.cy - 1].chars, prev_len + line->size + 1);
        memcpy(&E.lines[E.cy - 1].chars[prev_len], line->chars, line->size);
        E.lines[E.cy - 1].size += line->size;
        E.lines[E.cy - 1].chars[E.lines[E.cy - 1].size] = '\0';
        editorUpdateLine(&E.lines[E.cy - 1]);
        editorDelLine(E.cy);
        E.cy--;
    }
}

void editorFreeBuffer(void) {
    if (E.lines) {
        for (int i = 0; i < E.num_lines; i++) {
            editorFreeLine(&E.lines[i]);
        }
        free(E.lines);
        E.lines = NULL;
    }
    E.num_lines = 0;
}
