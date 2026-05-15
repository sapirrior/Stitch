#include <stdlib.h>
#include "stitch/types.h"
#include "stitch/ui/prompt.h"
#include "stitch/ui/render.h"
#include "stitch/editor/commands.h"
#include "../editor_internal.h"

void handle_command_prompt_mode(StitchState *state, int c) {
    (void)c; /* c is ':' but prompted here */
    char *cmd = ui_prompt(state, ":%s", NULL);
    if (cmd) {
        editor_add_history(state, cmd);
        editor_handle_command(state, cmd);
        free(cmd);
    }
    state->editor.mode = MODE_NORMAL;
}
