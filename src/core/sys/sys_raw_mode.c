#include <ncurses.h>
#include "stitch/types.h"
#include "../core_internal.h"

void sys_init_colors(void) {
    /* Monochrome-Plus: Use terminal defaults for background */
    start_color();
    use_default_colors();

    /* 1: Accent Color (e.g., White on Black, for mode labels) */
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    
    /* 2: Normal Text (Standard) */
    init_pair(4, -1, -1);
    
    /* 3: Gutter/Dimmed Text */
    init_pair(5, COLOR_BLACK, -1); /* Using Black on -1 (transparent) for dimmed effect */
    
    bkgd(COLOR_PAIR(4));
}

void sys_init_ncurses(StitchState *state) {
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    set_escdelay(50);
    start_color();
    sys_init_colors();
    (void)state;
}
