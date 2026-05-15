#include <ncurses.h>
#include "stitch/types.h"
#include "../core_internal.h"

void sys_init_colors(void) {
    if (can_change_color()) {
        /* Define Stitch-Forest Palette */
        init_color(10, 580, 680, 560); /* Sage Green */
        init_color(11, 840, 580, 480); /* Terracotta Orange */
        init_color(12, 940, 780, 380); /* Ochre Yellow */
        init_color(13, 160, 200, 220); /* Midnight Blue-Grey (Gutter/Status) */
        init_color(14, 980, 960, 940); /* Soft Cream */
        init_color(0, 180, 220, 240);  /* Dark Grey-Green (Base Background) */

        init_pair(1, 14, 10); /* Sage Accent */
        init_pair(2, 14, 11); /* Terracotta Accent */
        init_pair(3, 14, 12); /* Ochre Accent */
        init_pair(4, 14, 0);  /* Soft Cream on Base */
        init_pair(5, 14, 13); /* Soft Cream on Midnight */
    } else {
        /* Fallback for non-256 color terminals */
        init_pair(1, 237, 108);
        init_pair(2, 237, 173);
        init_pair(3, 237, 179);
        init_pair(4, 230, 235);
        init_pair(5, 230, 237);
    }
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
