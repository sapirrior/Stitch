#include <ncurses.h>
#include <string.h>
#include "stitch/types.h"
#include "stitch/core/terminal.h"
#include "ui_internal.h"

void ui_status_bar_draw(StitchState *state) {
    int mode_pair = 1;
    const char *mode_str = " NORMAL ";
    if (state->editor.mode == MODE_INSERT) { mode_pair = 2; mode_str = " INSERT "; }
    else if (state->editor.mode == MODE_COMMAND) { mode_pair = 3; mode_str = " COMMAND "; }

    int y = state->view.screen_rows;
    move(y, 0);
    attron(COLOR_PAIR(mode_pair));
    addstr(mode_str);
    attroff(COLOR_PAIR(mode_pair));

    attron(COLOR_PAIR(5)); /* Cream on Earth */
    char status[120], rstatus[120];
    int len = snprintf(status, sizeof(status), " %s %s",
        state->buffer.filename ? state->buffer.filename : "[No Name]",
        state->buffer.dirty ? "*" : "");
    snprintf(rstatus, sizeof(rstatus), " %d:%d ",
        state->view.cy + 1, state->view.cx + 1);

    int mode_len = (int)strlen(mode_str);
    int status_bytes = editorRowColToByte(status, len, state->view.screen_cols - mode_len - (int)strlen(rstatus) - 1);
    
    addnstr(status, status_bytes);
    
    int current_x = mode_len + editorRowByteToCol(status, len, status_bytes);
    while (current_x < state->view.screen_cols - (int)strlen(rstatus)) {
        addch(' ');
        current_x++;
    }

    attroff(COLOR_PAIR(5));
    attron(COLOR_PAIR(1)); /* Sage block for position */
    addstr(rstatus);
    attroff(COLOR_PAIR(1));
}
