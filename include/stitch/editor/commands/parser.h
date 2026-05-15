#ifndef STITCH_EDITOR_COMMANDS_PARSER_H
#define STITCH_EDITOR_COMMANDS_PARSER_H

#include "../../types.h"

void handleCommand(const char *cmd);
void editorUpdateShellStatus(void);
void editorAddHistory(const char *cmd);

#endif
