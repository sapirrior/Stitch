#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "stitch/buffer/io.h"
#include "stitch/buffer/engine.h"
#include "stitch/core/terminal.h"
#include "stitch/ui/render.h"
#include "stitch/ui/prompt.h"

static char *buffer_rows_to_string(StitchBuffer *buf, size_t *buflen) {
    size_t totlen = 0;
    for (size_t i = 0; i < buf->num_lines; i++)
        totlen += buf->lines[i].size + 1;
    *buflen = totlen;

    if (totlen == 0) return NULL;

    char *res = editorMalloc(totlen);
    char *p = res;
    for (size_t i = 0; i < buf->num_lines; i++) {
        memcpy(p, buf->lines[i].chars, buf->lines[i].size);
        p += buf->lines[i].size;
        *p = '\n';
        p++;
    }
    return res;
}

void editorSave(StitchState *state) {
    if (state->buffer.filename == NULL) {
        state->buffer.filename = ui_prompt(state, "Save as: %s", NULL);
        if (state->buffer.filename == NULL) {
            ui_set_status_message(state, "Save aborted");
            return;
        }
    }

    size_t len;
    char *buf = buffer_rows_to_string(&state->buffer, &len);

    size_t tmp_len = strlen(state->buffer.filename) + 6;
    char *tmp_filename = editorMalloc(tmp_len);
    snprintf(tmp_filename, tmp_len, ".%s.tmp", state->buffer.filename);

    FILE *fp = fopen(tmp_filename, "w");
    if (fp != NULL) {
        if (len == 0 || fwrite(buf, 1, len, fp) == len) {
            if (fclose(fp) == 0) {
                if (rename(tmp_filename, state->buffer.filename) == 0) {
                    free(buf);
                    free(tmp_filename);
                    state->buffer.dirty = 0;
                    ui_set_status_message(state, "%zu bytes written to disk", len);
                    return;
                }
            }
        } else {
            fclose(fp);
        }
    }

    if (tmp_filename) {
        unlink(tmp_filename);
        free(tmp_filename);
    }
    free(buf);
    ui_set_status_message(state, "Can't save! I/O error: %s", strerror(errno));
}

int editorOpen(StitchState *state, char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp && errno != ENOENT) return -1;

    buffer_free(&state->buffer);
    free(state->buffer.filename);
    state->buffer.filename = editorStrdup(filename);

    if (fp) {
        char *line = NULL;
        size_t linecap = 0;
        ssize_t linelen;
        while ((linelen = getline(&line, &linecap, fp)) != -1) {
            while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
                linelen--;
            buffer_insert_line(&state->buffer, state->buffer.num_lines, line, (size_t)linelen);
        }
        free(line);
        fclose(fp);
    }

    state->view.cx = 0;
    state->view.cy = 0;
    state->view.row_off = 0;
    state->view.col_off = 0;
    state->buffer.dirty = 0;

    return 0;
}
