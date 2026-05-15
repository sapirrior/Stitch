#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include "stitch/core/terminal.h"
#include "stitch/editor/modes.h"
#include "stitch/buffer/engine.h"
#include "stitch/ui/prompt.h"
#include "stitch/editor/commands/parser.h"

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

    if (E.mode == MODE_NORMAL) {
        /* Handle multi-key commands like dd */
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
                if (E.cy < E.num_lines) {
                    E.cx = E.lines[E.cy].size;
                }
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
                char *cmd = editorPrompt(":%s");
                if (cmd) {
                    handleCommand(cmd);
                    free(cmd);
                }
                E.mode = MODE_NORMAL;
                break;
            }
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
                    if (c == PAGE_UP) {
                        E.cy = E.row_off;
                    } else if (c == PAGE_DOWN) {
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
