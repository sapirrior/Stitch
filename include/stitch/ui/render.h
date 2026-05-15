#ifndef STITCH_UI_RENDER_H
#define STITCH_UI_RENDER_H

#include "stitch/types.h"

/* Organic Warmth Palette (Truecolor) */
#define STITCH_RESET      "\x1b[0m"
#define STITCH_FG_CREAM   "\x1b[38;2;247;243;232m"
#define STITCH_FG_EARTH   "\x1b[38;2;47;42;40m"
#define STITCH_BG_EARTH   "\x1b[48;2;47;42;40m"
#define STITCH_BG_SAGE    "\x1b[48;2;130;150;114m"
#define STITCH_BG_TERRA   "\x1b[48;2;195;122;103m"
#define STITCH_BG_OCHRE   "\x1b[48;2;215;160;75m"

/* RGB Hex Values */
#define RGB_CREAM 0xF7F3E8
#define RGB_EARTH 0x2F2A28
#define RGB_SAGE  0x829672
#define RGB_TERRA 0xC37A67
#define RGB_OCHRE 0xD7A04B

void editorInitGrid(int w, int h);
void editorResizeGrid(int w, int h);
void editorFreeGrid(void);
void gridClear(void);
void gridPutChar(int x, int y, const char *utf8, uint32_t fg, uint32_t bg);

void editorRefreshScreen(void);
void editorSetStatusMessage(const char *fmt, ...);
void editorHandleResize(void);
int getWindowSize(int *rows, int *cols);

#endif
