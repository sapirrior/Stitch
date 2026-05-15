#ifndef STITCH_UI_PROMPT_H
#define STITCH_UI_PROMPT_H

#include "stitch/types.h"

char *ui_prompt(StitchState *state, char *prompt, void (*callback)(StitchState *, char *, int));

#endif
