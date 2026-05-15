#include <stdlib.h>
#include "stitch/types.h"
#include "stitch/ui/render.h"
#include "../editor_internal.h"

void cmd_quit_execute(StitchState *state) {
    if (state->buffer.dirty) {
        ui_set_status_message(state, "No write since last change (add ! to override)");
        return;
    }
    exit(0);
}
