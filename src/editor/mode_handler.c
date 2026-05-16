#include "stitch/types.h"
#include "stitch/core/terminal.h"
#include "stitch/ui/render.h"
#include "stitch/editor/modes.h"
#include "editor_internal.h"

void editor_process_keypress(StitchState *state) {
    int c = core_read_key(state);

    if (c == STITCH_KEY_NONE) return;

    if (state->ui.show_help_overlay) {
        if (c == STITCH_KEY_RESIZE) {
            ui_handle_resize(state);
        } else if (c == '\x1b') {
            state->ui.show_help_overlay = false;
        }
        return;
    }

    if (c == STITCH_KEY_RESIZE) {
        ui_handle_resize(state);
        return;
    }

    if (state->editor.mode == MODE_NORMAL) {
        handle_normal_mode(state, c);
    } else if (state->editor.mode == MODE_INSERT) {
        handle_insert_mode(state, c);
    }
}
