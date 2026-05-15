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

char *editorPrompt(char *prompt) {
    size_t bufsize = 128;
    char *buf = malloc(bufsize);
    if (!buf) return NULL;
    size_t buflen = 0;
    buf[0] = '\0';

    while (1) {
        editorSetStatusMessage(prompt, buf);
        editorRefreshScreen();

        int c = editorReadKey();

        if (c == '\r') {
            if (buflen != 0) {
                editorSetStatusMessage("");
                return buf;
            }
        } else if (c == '\x1b') {
            editorSetStatusMessage("");
            free(buf);
            return NULL;
        } else if (c == BACKSPACE || c == CTRL_KEY('h') || c == DEL_KEY) {
            if (buflen != 0) buf[--buflen] = '\0';
        } else if (!iscntrl(c) && c < 128) {
            if (buflen == bufsize - 1) {
                bufsize *= 2;
                buf = realloc(buf, bufsize);
                if (!buf) return NULL;
            }
            buf[buflen++] = c;
            buf[buflen] = '\0';
        }
    }
}
