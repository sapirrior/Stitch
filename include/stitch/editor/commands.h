#ifndef STITCH_EDITOR_COMMANDS_H
#define STITCH_EDITOR_COMMANDS_H

#include "stitch/types.h"

void editor_handle_command(StitchState *state, const char *cmd);
void editor_update_shell_status(StitchState *state);
void editor_add_history(StitchState *state, const char *cmd);

#endif
