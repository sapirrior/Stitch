#include <stdlib.h>
#include <string.h>
#include "stitch/types.h"
#include "stitch/core/terminal.h"
#include "stitch/buffer/engine.h"
#include "stitch/ui/render.h"
#include "../editor_internal.h"

static void get_visual_bounds(StitchState *state, size_t *sy, size_t *sx, size_t *ey, size_t *ex) {
    if (state->view.cy < state->editor.visual_cy || (state->view.cy == state->editor.visual_cy && state->view.cx < state->editor.visual_cx)) {
        *sy = state->view.cy;
        *sx = state->view.cx;
        *ey = state->editor.visual_cy;
        *ex = state->editor.visual_cx;
    } else {
        *sy = state->editor.visual_cy;
        *sx = state->editor.visual_cx;
        *ey = state->view.cy;
        *ex = state->view.cx;
    }
}

static char *extract_visual_text(StitchState *state, size_t sy, size_t sx, size_t ey, size_t ex, size_t *out_len) {
    size_t cap = 256;
    size_t len = 0;
    char *buf = editorMalloc(cap);

    for (size_t y = sy; y <= ey && y < state->buffer.num_lines; y++) {
        Line *line = &state->buffer.lines[y];
        size_t c_start = (y == sy) ? sx : 0;
        size_t c_end = (y == ey) ? ex : line->size;
        
        size_t next_c = c_end;
        if (next_c < line->size) {
            next_c++;
            while (next_c < line->size && !editorIsUtf8Start((unsigned char)line->chars[next_c])) {
                next_c++;
            }
        }

        size_t chunk = next_c - c_start;
        if (len + chunk + 2 > cap) {
            cap = len + chunk + 256;
            buf = editorRealloc(buf, cap);
        }
        if (chunk > 0) {
            memcpy(buf + len, &line->chars[c_start], chunk);
            len += chunk;
        }
        
        if (y < ey || (y == ey && ex == line->size && y < state->buffer.num_lines - 1)) {
            buf[len++] = '\n';
        }
    }
    buf[len] = '\0';
    *out_len = len;
    return buf;
}

static void delete_visual_block(StitchState *state, size_t sy, size_t sx, size_t ey, size_t ex) {
    state->view.cy = ey;
    state->view.cx = ex;

    if (state->view.cy < state->buffer.num_lines) {
        Line *line = &state->buffer.lines[state->view.cy];
        if (state->view.cx < line->size) {
            state->view.cx++;
            while (state->view.cx < line->size && !editorIsUtf8Start((unsigned char)line->chars[state->view.cx])) {
                state->view.cx++;
            }
        } else {
            if (state->view.cy < state->buffer.num_lines - 1) {
                state->view.cy++;
                state->view.cx = 0;
            }
        }
    }

    while (true) {
        if (state->view.cy < sy || (state->view.cy == sy && state->view.cx <= sx)) break;
        
        if (state->view.cx > 0) {
            state->view.cx--;
            Line *line = &state->buffer.lines[state->view.cy];
            while (state->view.cx > 0 && !editorIsUtf8Start((unsigned char)line->chars[state->view.cx])) {
                state->view.cx--;
            }
        } else {
            state->view.cy--;
            state->view.cx = state->buffer.lines[state->view.cy].size;
        }

        buffer_del_char(&state->buffer, &state->view);
    }
    
    state->view.cy = sy;
    state->view.cx = sx;
}

void handle_visual_mode(StitchState *state, int c) {
    size_t sy, sx, ey, ex;
    size_t len;
    char *text;

    switch (c) {
        case '\x1b':
        case 'v':
            state->editor.mode = MODE_NORMAL;
            ui_set_status_message(state, "");
            break;
        case 'h':
        case 'j':
        case 'k':
        case 'l':
        case STITCH_ARROW_UP:
        case STITCH_ARROW_DOWN:
        case STITCH_ARROW_LEFT:
        case STITCH_ARROW_RIGHT:
            editor_move_cursor(state, c);
            break;
        case '0':
        case STITCH_HOME_KEY:
            state->view.cx = 0;
            break;
        case '$':
        case STITCH_END_KEY:
            if (state->view.cy < state->buffer.num_lines) state->view.cx = state->buffer.lines[state->view.cy].size;
            break;
        case 'g':
            state->view.cy = 0;
            state->view.cx = 0;
            break;
        case 'G':
            if (state->buffer.num_lines > 0) {
                state->view.cy = state->buffer.num_lines - 1;
            } else {
                state->view.cy = 0;
            }
            state->view.cx = 0;
            break;
        case 'y':
            get_visual_bounds(state, &sy, &sx, &ey, &ex);
            text = extract_visual_text(state, sy, sx, ey, ex, &len);
            sys_clipboard_set(text, len);
            free(text);
            state->editor.mode = MODE_NORMAL;
            ui_set_status_message(state, "%zu bytes yanked", len);
            break;
        case 'd':
        case 'x':
            get_visual_bounds(state, &sy, &sx, &ey, &ex);
            text = extract_visual_text(state, sy, sx, ey, ex, &len);
            sys_clipboard_set(text, len);
            free(text);
            delete_visual_block(state, sy, sx, ey, ex);
            state->editor.mode = MODE_NORMAL;
            ui_set_status_message(state, "");
            break;
    }
}
