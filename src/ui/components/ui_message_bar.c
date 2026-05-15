#include <ncurses.h>
#include <string.h>
#include "stitch/types.h"
#include "stitch/core/terminal.h"
#include "ui_internal.h"

void ui_message_bar_draw(StitchState *state) {
    move(state->view.screen_rows + 1, 0);
    clrtoeol();
    attron(COLOR_PAIR(4));
    int msg_len = (int)strlen(state->ui.status_msg);
    int msg_bytes = editorRowColToByte(state->ui.status_msg, msg_len, state->view.screen_cols);
    addnstr(state->ui.status_msg, msg_bytes);
    attroff(COLOR_PAIR(4));
}
