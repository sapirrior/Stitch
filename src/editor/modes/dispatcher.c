#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include "stitch/core/terminal.h"
#include "stitch/editor/modes.h"
#include "stitch/buffer/engine.h"
#include "stitch/ui/prompt.h"
#include "stitch/editor/commands/parser.h"

static void editorFindCallback(char *query, int key) {
    static int last_match = -1;
    static int direction = 1;

    free(E.search_query);
    E.search_query = NULL;

    if (key == '\r' || key == '\x1b') {
        last_match = -1;
        direction = 1;
        return;
    } else if (key == ARROW_RIGHT || key == ARROW_DOWN) {
        direction = 1;
    } else if (key == ARROW_LEFT || key == ARROW_UP) {
        direction = -1;
    } else {
        last_match = -1;
        direction = 1;
    }

    if (query[0] == '\0') return;
    E.search_query = editorStrdup(query);

    int total_matches = 0;
    int current_match_idx = -1;

    /* First pass: count total matches and find current index */
    for (int i = 0; i < E.num_lines; i++) {
        char *line_ptr = E.lines[i].render;
        while ((line_ptr = editorStrcasestr(line_ptr, query)) != NULL) {
            total_matches++;
            if (i == last_match) {
                /* Simplified: only tracks line index, not occurrence within line */
            }
            line_ptr += strlen(query);
        }
    }

    /* Second pass: find the match to jump to */
    int current = last_match;
    for (int i = 0; i < E.num_lines; i++) {
        current += direction;
        if (current == -1) current = E.num_lines - 1;
        else if (current == E.num_lines) current = 0;

        char *match = editorStrcasestr(E.lines[current].render, query);
        if (match) {
            last_match = current;
            E.cy = current;
            E.cx = 0;
            char *chars_match = editorStrcasestr(E.lines[current].chars, query);
            if (chars_match) E.cx = (int)(chars_match - E.lines[current].chars);
            E.row_off = E.num_lines; /* Force scroll */

            /* Calculate current_match_idx (approximate by line index for now) */
            int found_count = 0;
            for (int j = 0; j <= current; j++) {
                char *lp = E.lines[j].render;
                while ((lp = editorStrcasestr(lp, query)) != NULL) {
                    found_count++;
                    lp += strlen(query);
                }
            }
            current_match_idx = found_count;
            break;
        }
    }

    if (total_matches > 0) {
        editorSetStatusMessage("Match %d of %d (Arrows to navigate)", 
                               current_match_idx > 0 ? current_match_idx : 1, 
                               total_matches);
    } else {
        editorSetStatusMessage("No matches found");
    }
}

void editorFind(void) {
    int saved_cx = E.cx;
    int saved_cy = E.cy;
    int saved_col_off = E.col_off;
    int saved_row_off = E.row_off;

    char *query = editorPrompt("Search: %s (Use Arrows/Enter/Esc)",
                               editorFindCallback);

    if (query) {
        free(query);
    } else {
        E.cx = saved_cx;
        E.cy = saved_cy;
        E.col_off = saved_col_off;
        E.row_off = saved_row_off;
    }
}

void editorMoveCursor(int key) {
    Line *line = (E.cy >= E.num_lines) ? NULL : &E.lines[E.cy];

    switch (key) {
        case 'h':
        case ARROW_LEFT:
            if (E.cx > 0) {
                E.cx--;
            } else if (E.cy > 0) {
                E.cy--;
                E.cx = E.lines[E.cy].size;
            }
            break;
        case 'l':
        case ARROW_RIGHT:
            if (line && E.cx < line->size) {
                E.cx++;
            } else if (line && E.cx == line->size) {
                E.cy++;
                E.cx = 0;
            }
            break;
        case 'k':
        case ARROW_UP:
            if (E.cy > 0) E.cy--;
            break;
        case 'j':
        case ARROW_DOWN:
            if (E.cy < E.num_lines) E.cy++;
            break;
    }

    line = (E.cy >= E.num_lines) ? NULL : &E.lines[E.cy];
    int linelen = line ? line->size : 0;
    if (E.cx > linelen) E.cx = linelen;
}

void editorProcessKeypress(void) {
    int c = editorReadKey();

    if (c == KEY_NONE) return;

    if (E.mode == MODE_NORMAL) {
        if (E.last_key == 'd') {
            if (c == 'd') {
                editorDelLine(E.cy);
                if (E.cy == E.num_lines && E.cy > 0) E.cy--;
                E.cx = 0;
            }
            E.last_key = 0;
            return;
        }

        switch (c) {
            case 'i':
                E.mode = MODE_INSERT;
                editorSetStatusMessage("-- INSERT --");
                break;
            case 'a':
                if (E.cy < E.num_lines) {
                    Line *line = &E.lines[E.cy];
                    if (E.cx < line->size) E.cx++;
                }
                E.mode = MODE_INSERT;
                editorSetStatusMessage("-- INSERT --");
                break;
            case 'A':
                if (E.cy < E.num_lines) E.cx = E.lines[E.cy].size;
                E.mode = MODE_INSERT;
                editorSetStatusMessage("-- INSERT --");
                break;
            case 'o':
                E.cy++;
                editorInsertLine(E.cy, "", 0);
                E.cx = 0;
                E.mode = MODE_INSERT;
                editorSetStatusMessage("-- INSERT --");
                break;
            case 'O':
                editorInsertLine(E.cy, "", 0);
                E.cx = 0;
                E.mode = MODE_INSERT;
                editorSetStatusMessage("-- INSERT --");
                break;
            case ':': {
                E.mode = MODE_COMMAND;
                char *cmd = editorPrompt(":%s", NULL);
                if (cmd) {
                    editorAddHistory(cmd);
                    handleCommand(cmd);
                    free(cmd);
                }
                E.mode = MODE_NORMAL;
                break;
            }
            case '/':
                editorFind();
                break;
            case 'h':
            case 'j':
            case 'k':
            case 'l':
            case ARROW_UP:
            case ARROW_DOWN:
            case ARROW_LEFT:
            case ARROW_RIGHT:
                editorMoveCursor(c);
                break;
            case '0':
            case HOME_KEY:
                E.cx = 0;
                break;
            case '$':
            case END_KEY:
                if (E.cy < E.num_lines) E.cx = E.lines[E.cy].size;
                break;
            case 'g':
                E.cy = 0;
                E.cx = 0;
                break;
            case 'G':
                E.cy = E.num_lines - 1;
                if (E.cy < 0) E.cy = 0;
                E.cx = 0;
                break;
            case 'x':
                E.cx++;
                editorDelChar();
                break;
            case 'd':
                E.last_key = 'd';
                break;
            case PAGE_UP:
            case PAGE_DOWN:
                {
                    if (c == PAGE_UP) E.cy = E.row_off;
                    else if (c == PAGE_DOWN) {
                        E.cy = E.row_off + E.screen_rows - 1;
                        if (E.cy > E.num_lines) E.cy = E.num_lines;
                    }
                    int times = E.screen_rows;
                    while (times--)
                        editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
                }
                break;
        }
    } else if (E.mode == MODE_INSERT) {
        if (c == '\x1b') {
            E.mode = MODE_NORMAL;
            editorSetStatusMessage("");
        } else if (c == BACKSPACE || c == CTRL_KEY('h')) {
            editorDelChar();
        } else if (c == ARROW_UP || c == ARROW_DOWN || c == ARROW_LEFT || c == ARROW_RIGHT) {
            editorMoveCursor(c);
        } else if (c == '\r') {
            editorInsertNewline();
        } else if (!iscntrl(c)) {
            editorInsertChar(c);
        }
    }
}
