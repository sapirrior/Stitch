#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <ncurses.h>
#include "stitch/core/terminal.h"
#include "stitch/ui/prompt.h"
#include "stitch/ui/render.h"

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

        if (c == STITCH_DEL_KEY || c == CTRL_KEY('h') || c == STITCH_BACKSPACE) {
            if (buflen != 0) buf[--buflen] = '\0';
        } else if (c == '\x1b') {
            editorSetStatusMessage("");
            if (callback) callback(buf, c);
            free(buf);
            free(saved_buf);
            return NULL;
        } else if (c == STITCH_ARROW_UP || c == STITCH_ARROW_DOWN) {
            if (E.history_count == 0) continue;

            if (saved_buf == NULL) saved_buf = editorStrdup(buf);

            if (c == STITCH_ARROW_UP) {
                if (history_idx > 0) history_idx--;
            } else {
                if (history_idx < E.history_count) history_idx++;
            }

            free(buf);
            if (history_idx < E.history_count && E.history[history_idx]) {
                bufsize = strlen(E.history[history_idx]) + 1;
                buf = editorMalloc(bufsize);
                strcpy(buf, E.history[history_idx]);
            } else {
                bufsize = strlen(saved_buf) + 1;
                buf = editorMalloc(bufsize);
                strcpy(buf, saved_buf);
            }
            buflen = strlen(buf);
        } else if (c == '\r') {
            editorSetStatusMessage("");
            if (callback) callback(buf, c);
            free(saved_buf);
            return buf;
        } else if (c >= 32 && c < 127) {
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
