#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "stitch/buffer/io.h"
#include "stitch/buffer/engine.h"
#include "stitch/core/terminal.h"
#include "stitch/ui/prompt.h"

static char *editorRowsToString(int *buflen) {
    int totlen = 0;
    for (int i = 0; i < E.num_lines; i++)
        totlen += E.lines[i].size + 1;
    *buflen = totlen;

    if (totlen == 0) return NULL;

    char *buf = malloc(totlen);
    if (!buf) die("malloc");
    char *p = buf;
    for (int i = 0; i < E.num_lines; i++) {
        memcpy(p, E.lines[i].chars, E.lines[i].size);
        p += E.lines[i].size;
        *p = '\n';
        p++;
    }

    return buf;
}

void editorSave(void) {
    if (E.filename == NULL) {
        E.filename = editorPrompt("Save as: %s", NULL);
        if (E.filename == NULL) {
            editorSetStatusMessage("Save aborted");
            return;
        }
    }

    int len;
    char *buf = editorRowsToString(&len);

    FILE *fp = fopen(E.filename, "w");
    if (fp != NULL) {
        if (len == 0 || fwrite(buf, 1, len, fp) == (size_t)len) {
            fclose(fp);
            free(buf);
            E.dirty = 0;
            editorSetStatusMessage("%d bytes written to disk", len);
            return;
        }
        fclose(fp);
    }

    free(buf);
    editorSetStatusMessage("Can't save! I/O error: %s", strerror(errno));
}

int editorOpen(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp && errno != ENOENT) return -1;

    editorFreeBuffer();
    free(E.filename);
    E.filename = strdup(filename);

    if (fp) {
        char *line = NULL;
        size_t linecap = 0;
        ssize_t linelen;
        while ((linelen = getline(&line, &linecap, fp)) != -1) {
            while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
                linelen--;
            editorInsertLine(E.num_lines, line, linelen);
        }
        free(line);
        fclose(fp);
    }

    E.cx = 0;
    E.cy = 0;
    E.row_off = 0;
    E.col_off = 0;
    E.dirty = 0;

    return 0;
}
