#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include "stitch/core/terminal.h"
#include "stitch/ui/prompt.h"
#include "stitch/ui/render.h"

void editorSetStatusMessage(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(E.status_msg, sizeof(E.status_msg), fmt, ap);
    va_end(ap);
}

char *editorPrompt(char *prompt, void (*callback)(char *, int)) {
    size_t bufsize = 128;
    char *buf = editorMalloc(bufsize);
    size_t buflen = 0;
    buf[0] = '\0';

    int history_idx = E.history_count;
    char *saved_buf = NULL;

    while (1) {
        editorSetStatusMessage(prompt, buf);
        editorRefreshScreen();

        int c = editorReadKey();

        if (c == KEY_RESIZE || c == KEY_NONE) continue;

        if (c == ARROW_UP || c == ARROW_DOWN) {
            if (E.history_count == 0) continue;

            if (c == ARROW_UP) {
                if (history_idx == E.history_count) {
                    saved_buf = editorStrdup(buf);
                }
                if (history_idx > 0) {
                    history_idx--;
                    free(buf);
                    buf = editorStrdup(E.history[history_idx]);
                    buflen = strlen(buf);
                    bufsize = buflen + 1;
                }
            } else { /* ARROW_DOWN */
                if (history_idx < E.history_count) {
                    history_idx++;
                    free(buf);
                    if (history_idx == E.history_count) {
                        buf = (saved_buf) ? saved_buf : editorStrdup("");
                        saved_buf = NULL;
                    } else {
                        buf = editorStrdup(E.history[history_idx]);
                    }
                    buflen = strlen(buf);
                    bufsize = buflen + 1;
                }
            }
        } else if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE) {
            if (buflen != 0) buf[--buflen] = '\0';
        } else if (c == '\x1b') {
            editorSetStatusMessage("");
            if (callback) callback(buf, c);
            free(buf);
            free(saved_buf);
            return NULL;
        } else if (c == '\r') {
            if (buflen != 0) {
                editorSetStatusMessage("");
                if (callback) callback(buf, c);
                free(saved_buf);
                return buf;
            }
        } else if (!iscntrl(c) && c < 128) {
            if (buflen == bufsize - 1) {
                bufsize *= 2;
                buf = editorRealloc(buf, bufsize);
            }
            buf[buflen++] = (char)c;
            buf[buflen] = '\0';
        }

        if (callback) callback(buf, c);
    }
}
