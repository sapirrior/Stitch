#include <ncurses.h>
#include "stitch/types.h"
#include "../core_internal.h"

void sys_init_colors(void) {
    if (can_change_color()) {
        init_color(10, 510, 588, 447); /* Sage */
        init_color(11, 765, 478, 404); /* Terra */
        init_color(12, 843, 627, 294); /* Ochre */
        init_color(13, 184, 165, 157); /* Earth */
        init_color(14, 968, 953, 910); /* Cream */
        
        init_pair(1, 13, 10);
        init_pair(2, 13, 11);
        init_pair(3, 13, 12);
        init_pair(4, 14, 0);
        init_pair(5, 14, 13);
    } else {
        init_pair(1, 235, 108);
        init_pair(2, 235, 173);
        init_pair(3, 235, 179);
        init_pair(4, 230, 0);
        init_pair(5, 230, 235);
    }
    bkgd(COLOR_PAIR(4));
}

void sys_init_ncurses(StitchState *state) {
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    start_color();
    sys_init_colors();
    (void)state;
}
