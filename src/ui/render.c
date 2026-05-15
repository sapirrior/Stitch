#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "stitch/core/terminal.h"
#include "stitch/ui/render.h"

struct abuf {
    char *b;
    int len;
};

#define ABUF_INIT {NULL, 0}

static void abAppend(struct abuf *ab, const char *s, int len) {
    char *new = editorRealloc(ab->b, ab->len + len);
    memcpy(&new[ab->len], s, len);
    ab->b = new;
    ab->len += len;
}

static void abFree(struct abuf *ab) {
    free(ab->b);
}

void editorScroll(void) {
    E.rx = 0;
    if (E.cy < E.num_lines) {
        for (int j = 0; j < E.cx; j++) {
            if (E.lines[E.cy].chars[j] == '\t')
                E.rx += (STITCH_TAB_STOP - (E.rx % STITCH_TAB_STOP)) - 1;
            E.rx++;
        }
    }

    if (E.cy < E.row_off) {
        E.row_off = E.cy;
    }
    if (E.cy >= E.row_off + E.screen_rows) {
        E.row_off = E.cy - E.screen_rows + 1;
    }
    if (E.rx < E.col_off) {
        E.col_off = E.rx;
    }
    if (E.rx >= E.col_off + E.screen_cols) {
        E.col_off = E.rx - E.screen_cols + 1;
    }
}

static void editorDrawRows(struct abuf *ab) {
    abAppend(ab, STITCH_FG_CREAM, (int)strlen(STITCH_FG_CREAM));

    for (int y = 0; y < E.screen_rows; y++) {
        int filerow = y + E.row_off;
        if (filerow >= E.num_lines) {
            if (E.num_lines == 0 && y == E.screen_rows / 3) {
                char welcome[80];
                int welcomelen = snprintf(welcome, sizeof(welcome),
                    "Stitch editor -- version %s", STITCH_VERSION);
                if (welcomelen > E.screen_cols) welcomelen = E.screen_cols;
                int padding = (E.screen_cols - welcomelen) / 2;
                while (padding--) abAppend(ab, " ", 1);
                abAppend(ab, welcome, welcomelen);
            }
        } else {
            int len = E.lines[filerow].rsize - E.col_off;
            if (len < 0) len = 0;
            if (len > E.screen_cols) len = E.screen_cols;
            abAppend(ab, &E.lines[filerow].render[E.col_off], len);
        }

        abAppend(ab, "\x1b[K", 3);
        abAppend(ab, "\r\n", 2);
    }
    abAppend(ab, STITCH_RESET, (int)strlen(STITCH_RESET));
}

static void editorDrawStatusBar(struct abuf *ab) {
    abAppend(ab, STITCH_BG_EARTH, (int)strlen(STITCH_BG_EARTH));
    abAppend(ab, STITCH_FG_CREAM, (int)strlen(STITCH_FG_CREAM));

    const char *mode_str = " NORMAL ";
    const char *mode_bg = STITCH_BG_SAGE;

    if (E.mode == MODE_INSERT) {
        mode_str = " INSERT ";
        mode_bg = STITCH_BG_TERRA;
    } else if (E.mode == MODE_COMMAND) {
        mode_str = " COMMAND ";
        mode_bg = STITCH_BG_OCHRE;
    }

    abAppend(ab, mode_bg, (int)strlen(mode_bg));
    abAppend(ab, STITCH_FG_EARTH, (int)strlen(STITCH_FG_EARTH));
    abAppend(ab, mode_str, (int)strlen(mode_str));

    abAppend(ab, STITCH_BG_EARTH, (int)strlen(STITCH_BG_EARTH));
    abAppend(ab, STITCH_FG_CREAM, (int)strlen(STITCH_FG_CREAM));

    char status[120], rstatus[120];
    int len = snprintf(status, sizeof(status), " %s %s",
        E.filename ? E.filename : "[No Name]",
        E.dirty ? "*" : "");
    
    int rlen = snprintf(rstatus, sizeof(rstatus), " %d:%d ",
        E.cy + 1, E.cx + 1);
    
    int current_len = (int)strlen(mode_str);
    if (len + current_len > E.screen_cols) len = E.screen_cols - current_len;
    if (len > 0) {
        abAppend(ab, status, len);
        current_len += len;
    }
    
    while (current_len < E.screen_cols) {
        if (E.screen_cols - current_len == rlen) {
            abAppend(ab, rstatus, rlen);
            break;
        } else {
            abAppend(ab, " ", 1);
            current_len++;
        }
    }

    abAppend(ab, STITCH_RESET, (int)strlen(STITCH_RESET));
    abAppend(ab, "\r\n", 2);
}

static void editorDrawMessageBar(struct abuf *ab) {
    abAppend(ab, "\x1b[K", 3);
    int msglen = (int)strlen(E.status_msg);
    if (msglen > E.screen_cols) msglen = E.screen_cols;
    
    if (msglen > 0) {
        abAppend(ab, STITCH_FG_CREAM, (int)strlen(STITCH_FG_CREAM));
        abAppend(ab, E.status_msg, msglen);
        abAppend(ab, STITCH_RESET, (int)strlen(STITCH_RESET));
    }
}

void editorHandleResize(void) {
    if (E.resize_pending) {
        E.resize_pending = 0;
        if (getWindowSize(&E.screen_rows, &E.screen_cols) == -1) die("getWindowSize");
        
        if (E.screen_rows < 3) E.screen_rows = 3;
        if (E.screen_cols < 10) E.screen_cols = 10;

        E.screen_rows -= 2; /* Status bar + Message bar */
        write(STDOUT_FILENO, "\x1b[2J", 4);
    }
}

void editorRefreshScreen(void) {
    editorHandleResize();
    editorScroll();

    struct abuf ab = ABUF_INIT;

    abAppend(&ab, "\x1b[?25l", 6);
    abAppend(&ab, "\x1b[H", 3);

    editorDrawRows(&ab);
    editorDrawStatusBar(&ab);
    editorDrawMessageBar(&ab);

    char buf[32];
    if (E.mode == MODE_COMMAND) {
        snprintf(buf, sizeof(buf), "\x1b[%d;%dH", E.screen_rows + 2,
                                                  (int)strlen(E.status_msg) + 1);
    } else {
        snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (E.cy - E.row_off) + 1,
                                                  (E.rx - E.col_off) + 1);
    }
    abAppend(&ab, buf, (int)strlen(buf));

    abAppend(&ab, "\x1b[?25h", 6);

    write(STDOUT_FILENO, ab.b, ab.len);
    abFree(&ab);
}
