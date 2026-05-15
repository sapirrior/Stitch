#ifndef STITCH_UI_PROMPT_H
#define STITCH_UI_PROMPT_H

#include "../types.h"

void editorSetStatusMessage(const char *fmt, ...);
char *editorPrompt(char *prompt);

#endif
