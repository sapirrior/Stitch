#include <ncurses.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "stitch/types.h"
#include "stitch/core/terminal.h"
#include "ui_internal.h"

void ui_update_viewport(StitchState *state) {
    state->view.rx = 0;
    if (state->view.cy < state->buffer.num_lines) {
        Line *line = &state->buffer.lines[state->view.cy];
        for (int j = 0; j < state->view.cx && j < line->size; j++) {
            unsigned char c = (unsigned char)line->chars[j];
            if (c == '\t') {
                state->view.rx += (STITCH_TAB_STOP - (state->view.rx % STITCH_TAB_STOP));
            } else if (c < 32 || c == 127) {
                state->view.rx += 2;
            } else if (editorIsUtf8Start(c)) {
                state->view.rx++;
            }
        }
    }

    if (state->view.cy < state->view.row_off) {
        state->view.row_off = state->view.cy;
    }
    if (state->view.cy >= state->view.row_off + state->view.screen_rows) {
        state->view.row_off = state->view.cy - state->view.screen_rows + 1;
    }
    if (state->view.rx < state->view.col_off) {
        state->view.col_off = state->view.rx;
    }
    if (state->view.rx >= state->view.col_off + state->view.screen_cols) {
        state->view.col_off = state->view.rx - state->view.screen_cols + 1;
    }
}

void ui_refresh_screen(StitchState *state) {
    ui_update_viewport(state);

    ui_text_grid_draw(state);
    ui_status_bar_draw(state);
    ui_message_bar_draw(state);

    /* Position cursor */
    if (state->editor.mode == MODE_COMMAND) {
        int msg_cols = editorRowByteToCol(state->ui.status_msg, (int)strlen(state->ui.status_msg), (int)strlen(state->ui.status_msg));
        move(state->view.screen_rows + 1, msg_cols);
    } else {
        move(state->view.cy - state->view.row_off, state->view.rx - state->view.col_off);
    }

    wnoutrefresh(stdscr);
    doupdate();
}

void ui_set_status_message(StitchState *state, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(state->ui.status_msg, sizeof(state->ui.status_msg), fmt, ap);
    va_end(ap);
}

void ui_handle_resize(StitchState *state) {
    resizeterm(0, 0);
    clear();
    getmaxyx(stdscr, state->view.screen_rows, state->view.screen_cols);
    state->view.screen_rows -= 2;
    if (state->view.screen_rows < 0) state->view.screen_rows = 0;
    touchwin(stdscr);
}
