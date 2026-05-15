#ifndef STITCH_UI_RENDER_H
#define STITCH_UI_RENDER_H

#include "stitch/types.h"

/* RGB Hex Values for Palette */
#define RGB_CREAM 0xF7F3E8
#define RGB_EARTH 0x2F2A28
#define RGB_SAGE  0x829672
#define RGB_TERRA 0xC37A67
#define RGB_OCHRE 0xD7A04B

void ui_refresh_screen(StitchState *state);
void ui_set_status_message(StitchState *state, const char *fmt, ...);
void ui_handle_resize(StitchState *state);

#endif
