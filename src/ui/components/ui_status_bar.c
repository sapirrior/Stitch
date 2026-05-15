#include <ncurses.h>
#include <string.h>
#include "stitch/types.h"
#include "stitch/core/terminal.h"
#include "ui_internal.h"

void ui_status_bar_draw(StitchState *state) {
    const char *mode_str = " NORMAL ";
    if (state->editor.mode == MODE_INSERT) mode_str = " INSERT ";
    else if (state->editor.mode == MODE_COMMAND) mode_str = " COMMAND ";

    int y = state->view.screen_rows;
    move(y, 0);

    attron(A_REVERSE);
    addstr(mode_str);
    attroff(A_REVERSE);

    char status[120], rstatus[120];
    int len = snprintf(status, sizeof(status), " %s%s",
        state->buffer.filename ? state->buffer.filename : "[No Name]",
        state->buffer.dirty ? "*" : "");
    snprintf(rstatus, sizeof(rstatus), " %zu:%zu ",
        state->view.cy + 1, state->view.cx + 1);

    int mode_len = (int)strlen(mode_str);
    int status_bytes = editorRowColToByte(status, len, state->view.screen_cols - mode_len - (int)strlen(rstatus) - 1);

    addnstr(status, status_bytes);

    /* Right aligned status */
    int rstatus_len = (int)strlen(rstatus);
    move(y, state->view.screen_cols - rstatus_len);
    addstr(rstatus);
}
