#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ncurses.h>
#include "stitch/core/terminal.h"
#include "stitch/ui/render.h"

void editorScroll(void) {
    E.rx = 0;
    if (E.cy < E.num_lines) {
        Line *line = &E.lines[E.cy];
        for (int j = 0; j < E.cx && j < line->size; j++) {
            unsigned char c = (unsigned char)line->chars[j];
            if (c == '\t') {
                E.rx += (STITCH_TAB_STOP - (E.rx % STITCH_TAB_STOP));
            } else if (c < 32 || c == 127) {
                E.rx += 2;
            } else if (editorIsUtf8Start(c)) {
                E.rx++;
            }
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

static void editorDrawRows(void) {
    attron(COLOR_PAIR(4)); /* Cream on Earth */
    for (int y = 0; y < E.screen_rows; y++) {
        int filerow = y + E.row_off;
        move(y, 0);
        clrtoeol();

        if (filerow >= E.num_lines) {
            if (E.num_lines == 0 && y == E.screen_rows / 3) {
                char welcome[80];
                int welcomelen = snprintf(welcome, sizeof(welcome),
                    "Stitch editor -- version %s", STITCH_VERSION);
                if (welcomelen > E.screen_cols) welcomelen = E.screen_cols;
                int x = (E.screen_cols - welcomelen) / 2;
                mvaddnstr(y, x, welcome, welcomelen);
            }
        } else {
            int rlen = E.lines[filerow].rsize;
            char *rline = E.lines[filerow].render;

            int coff = editorRowColToByte(rline, rlen, E.col_off);
            int draw_len = editorRowColToByte(&rline[coff], rlen - coff, E.screen_cols);

            if (E.search_query && E.search_query[0] != '\0' && draw_len > 0) {
                char *draw_ptr = &rline[coff];
                int current_idx = 0;
                int query_len = (int)strlen(E.search_query);

                while (current_idx < draw_len) {
                    char *match = editorStrcasestr(&draw_ptr[current_idx], E.search_query);
                    if (match && (match - draw_ptr) < draw_len) {
                        int match_idx = (int)(match - draw_ptr);
                        addnstr(&draw_ptr[current_idx], match_idx - current_idx);
                        
                        int qlen = query_len;
                        if (match_idx + qlen > draw_len) qlen = draw_len - match_idx;
                        
                        attron(COLOR_PAIR(3)); /* Ochre (Highlight) */
                        addnstr(match, qlen);
                        attroff(COLOR_PAIR(3));
                        attron(COLOR_PAIR(4));
                        
                        current_idx = match_idx + qlen;
                        if (qlen == 0) break; /* Avoid infinite loop */
                    } else {
                        addnstr(&draw_ptr[current_idx], draw_len - current_idx);
                        break;
                    }
                }
            } else {
                addnstr(&rline[coff], draw_len);
            }
        }
    }
    attroff(COLOR_PAIR(4));
}

static void editorDrawStatusBar(void) {
    int mode_pair = 1;
    const char *mode_str = " NORMAL ";
    if (E.mode == MODE_INSERT) { mode_pair = 2; mode_str = " INSERT "; }
    else if (E.mode == MODE_COMMAND) { mode_pair = 3; mode_str = " COMMAND "; }

    int y = E.screen_rows;
    move(y, 0);
    attron(COLOR_PAIR(mode_pair));
    addstr(mode_str);
    attroff(COLOR_PAIR(mode_pair));

    attron(COLOR_PAIR(5)); /* Cream on Earth */
    char status[120], rstatus[120];
    int len = snprintf(status, sizeof(status), " %s %s",
        E.filename ? E.filename : "[No Name]",
        E.dirty ? "*" : "");
    snprintf(rstatus, sizeof(rstatus), " %d:%d ",
        E.cy + 1, E.cx + 1);

    int mode_len = (int)strlen(mode_str);
    int status_bytes = editorRowColToByte(status, len, E.screen_cols - mode_len - (int)strlen(rstatus) - 1);
    
    addnstr(status, status_bytes);
    
    int current_x = mode_len + editorRowByteToCol(status, len, status_bytes);
    while (current_x < E.screen_cols - (int)strlen(rstatus)) {
        addch(' ');
        current_x++;
    }

    attroff(COLOR_PAIR(5));
    attron(COLOR_PAIR(1)); /* Sage block for position */
    addstr(rstatus);
    attroff(COLOR_PAIR(1));
}

static void editorDrawMessageBar(void) {
    move(E.screen_rows + 1, 0);
    clrtoeol();
    attron(COLOR_PAIR(4));
    int msg_len = (int)strlen(E.status_msg);
    int msg_bytes = editorRowColToByte(E.status_msg, msg_len, E.screen_cols);
    addnstr(E.status_msg, msg_bytes);
    attroff(COLOR_PAIR(4));
}

void editorRefreshScreen(void) {
    editorScroll();

    editorDrawRows();
    editorDrawStatusBar();
    editorDrawMessageBar();

    /* Position cursor */
    if (E.mode == MODE_COMMAND) {
        int msg_cols = editorRowByteToCol(E.status_msg, (int)strlen(E.status_msg), (int)strlen(E.status_msg));
        move(E.screen_rows + 1, msg_cols);
    } else {
        move(E.cy - E.row_off, E.rx - E.col_off);
    }

    wnoutrefresh(stdscr);
    doupdate();
}

void editorSetStatusMessage(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(E.status_msg, sizeof(E.status_msg), fmt, ap);
    va_end(ap);
}

void editorHandleResize(void) {
    resizeterm(0, 0);
    clear();
    getmaxyx(stdscr, E.screen_rows, E.screen_cols);
    E.screen_rows -= 2;
    if (E.screen_rows < 0) E.screen_rows = 0;
    touchwin(stdscr);
}
