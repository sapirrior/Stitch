#ifndef STITCH_TYPES_H
#define STITCH_TYPES_H

#define _POSIX_C_SOURCE 200809L
#include <termios.h>
#include <stddef.h>
#include <ctype.h>
#include <sys/types.h>

#define CTRL_KEY(k) ((k) & 0x1f)
#define STITCH_VERSION "0.1.0"
#define STITCH_TAB_STOP 8

typedef enum {
    MODE_NORMAL,
    MODE_INSERT,
    MODE_COMMAND
} Mode;

enum EditorKey {
    BACKSPACE = 127,
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    DEL_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN
};

typedef struct {
    int size;
    int rsize;
    char *chars;
    char *render;
} Line;

typedef struct {
    int cx, cy;
    int rx;
    int screen_rows;
    int screen_cols;
    int row_off;
    int col_off;
    int num_lines;
    Line *lines;
    char *filename;
    char status_msg[80];
    int dirty;
    Mode mode;
    int last_key;
    struct termios orig_termios;
} EditorConfig;

extern EditorConfig E;

#endif
