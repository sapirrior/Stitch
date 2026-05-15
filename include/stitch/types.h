#ifndef STITCH_TYPES_H
#define STITCH_TYPES_H

#define _POSIX_C_SOURCE 200809L
#include <termios.h>
#include <stddef.h>
#include <ctype.h>
#include <sys/types.h>
#include <signal.h>
#include <stdint.h>
#include <stdbool.h>
#include <ncurses.h>

#define CTRL_KEY(k) ((k) & 0x1f)
#define STITCH_VERSION "0.1.0"
#define STITCH_TAB_STOP 8

typedef enum {
    MODE_NORMAL,
    MODE_INSERT,
    MODE_COMMAND
} Mode;

enum EditorKey {
    STITCH_KEY_NONE = 0,
    STITCH_BACKSPACE = KEY_BACKSPACE,
    STITCH_ARROW_LEFT = KEY_LEFT,
    STITCH_ARROW_RIGHT = KEY_RIGHT,
    STITCH_ARROW_UP = KEY_UP,
    STITCH_ARROW_DOWN = KEY_DOWN,
    STITCH_DEL_KEY = KEY_DC,
    STITCH_HOME_KEY = KEY_HOME,
    STITCH_END_KEY = KEY_END,
    STITCH_PAGE_UP = KEY_PPAGE,
    STITCH_PAGE_DOWN = KEY_NPAGE,
    STITCH_KEY_RESIZE = KEY_RESIZE
};

typedef struct {
    size_t size;
    size_t rsize;
    size_t capacity;
    size_t rcapacity;
    char *chars;
    char *render;
} Line;

/* --- Domain Specific Structs --- */

typedef struct {
    Line *lines;
    size_t num_lines;
    char *filename;
    int dirty;
} StitchBuffer;

typedef struct {
    size_t cx, cy;
    size_t rx;
    int screen_rows;
    int screen_cols;
    size_t row_off;
    size_t col_off;
} StitchView;

typedef struct {
    char status_msg[80];
} StitchUI;

typedef struct {
    Mode mode;
    int last_key;
    char *search_query;
    char *history[10];
    int history_count;
} StitchEditor;

typedef struct {
    struct termios orig_termios;
    pid_t shell_pid;
} StitchCore;

/* --- Unified Application Context --- */

typedef struct {
    StitchBuffer buffer;
    StitchView view;
    StitchUI ui;
    StitchEditor editor;
    StitchCore core;
} StitchState;

#endif
