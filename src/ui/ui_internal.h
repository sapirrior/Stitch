#ifndef STITCH_UI_INTERNAL_H
#define STITCH_UI_INTERNAL_H

#include "stitch/types.h"

/* Component-level drawing functions */
void ui_status_bar_draw(StitchState *state);
void ui_message_bar_draw(StitchState *state);
void ui_text_grid_draw(StitchState *state);

/* Helper for scrolling and cursor positioning */
void ui_update_viewport(StitchState *state);

#endif
