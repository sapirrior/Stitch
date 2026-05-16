#include <ctype.h>
#include "stitch/types.h"
#include "stitch/buffer/engine.h"
#include "stitch/ui/render.h"
#include "../editor_internal.h"

void handle_insert_mode(StitchState *state, int c) {
    if (c == '\x1b') {
        state->editor.mode = MODE_NORMAL;
        ui_set_status_message(state, "");
    } else if (c == STITCH_BACKSPACE || c == CTRL_KEY('h')) {
        buffer_del_char(&state->buffer, &state->view);
    } else if (c == STITCH_ARROW_UP || c == STITCH_ARROW_DOWN || c == STITCH_ARROW_LEFT || c == STITCH_ARROW_RIGHT) {
        editor_move_cursor(state, c);
    } else if (c == '\r') {
        buffer_insert_newline(&state->buffer, &state->view);
    } else if (c == '\t') {
        buffer_insert_char(&state->buffer, &state->view, '\t');
    } else if (!iscntrl(c)) {
        buffer_insert_char(&state->buffer, &state->view, c);
    }
}
