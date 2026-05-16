#include <ncurses.h>
#include <string.h>
#include "stitch/types.h"
#include "../ui_internal.h"

static void help_print_line(int start_y, int start_x, int screen_rows, int line, const char *text, bool title) {
    if (start_y + line >= screen_rows - 1) return;
    move(start_y + line, start_x + 2);
    if (title) {
        attron(A_BOLD | COLOR_PAIR(1)); /* Sage for titles */
        addstr(text);
        attroff(A_BOLD | COLOR_PAIR(1));
    } else {
        addstr(text);
    }
}

void ui_help_overlay_draw(StitchState *state) {
    if (!state->ui.show_help_overlay) return;

    /* Dynamically adjust width and height based on terminal size */
    int width = state->view.screen_cols - 8;
    if (width > 64) width = 64; /* Cap width for readability */
    if (width < 40) width = state->view.screen_cols;

    int height = state->view.screen_rows - 4;
    if (height > 30) height = 30; /* Cap height to a reasonable size */
    if (height < 20) height = state->view.screen_rows;
    
    int start_y = (state->view.screen_rows - height) / 2;
    if (start_y < 0) start_y = 0;
    int start_x = (state->view.screen_cols - width) / 2;
    if (start_x < 0) start_x = 0;

    attron(COLOR_PAIR(4)); /* Standard Text */

    /* Draw Box */
    for (int y = 0; y < height; y++) {
        if (start_y + y >= state->view.screen_rows) break;
        move(start_y + y, start_x);
        if (y == 0) {
            addch(ACS_ULCORNER);
            for (int x = 1; x < width - 1; x++) addch(ACS_HLINE);
            addch(ACS_URCORNER);
        } else if (y == height - 1) {
            addch(ACS_LLCORNER);
            for (int x = 1; x < width - 1; x++) addch(ACS_HLINE);
            addch(ACS_LRCORNER);
        } else {
            addch(ACS_VLINE);
            for (int x = 1; x < width - 1; x++) addch(' ');
            addch(ACS_VLINE);
        }
    }

    int rows = state->view.screen_rows;
    int line = 1;

    help_print_line(start_y, start_x, rows, line++, "STITCH - THE GENTLE MODAL EDITOR", true);
    line++;
    
    help_print_line(start_y, start_x, rows, line++, "MODES", true);
    help_print_line(start_y, start_x, rows, line++, "  i / a / A - Insert / Append / End-Append", false);
    help_print_line(start_y, start_x, rows, line++, "  v         - Visual Mode", false);
    help_print_line(start_y, start_x, rows, line++, "  Esc       - Return to Normal Mode", false);
    help_print_line(start_y, start_x, rows, line++, "  :         - Enter Command Mode", false);
    line++;

    help_print_line(start_y, start_x, rows, line++, "MOVEMENT", true);
    help_print_line(start_y, start_x, rows, line++, "  h / j / k / l   - Left / Down / Up / Right", false);
    help_print_line(start_y, start_x, rows, line++, "  0 / $           - Start / End of Line", false);
    help_print_line(start_y, start_x, rows, line++, "  g / G / PgUp/Dn - Top / Bottom / Scroll", false);
    line++;

    help_print_line(start_y, start_x, rows, line++, "EDITING", true);
    help_print_line(start_y, start_x, rows, line++, "  x / dd          - Delete Char / Line", false);
    help_print_line(start_y, start_x, rows, line++, "  o / O           - New Line (Below / Above)", false);
    help_print_line(start_y, start_x, rows, line++, "  u / U           - Smart Undo / Redo", false);
    help_print_line(start_y, start_x, rows, line++, "  /               - Search Text", false);
    line++;

    help_print_line(start_y, start_x, rows, line++, "VISUAL MODE", true);
    help_print_line(start_y, start_x, rows, line++, "  d / x           - Delete Selection", false);
    help_print_line(start_y, start_x, rows, line++, "  %               - Select Entire Buffer", false);
    line++;

    help_print_line(start_y, start_x, rows, line++, "COMMANDS", true);
    help_print_line(start_y, start_x, rows, line++, "  :h / :help      - Show this help menu", false);
    help_print_line(start_y, start_x, rows, line++, "  :nu / :nonu     - Line Numbers (On / Off)", false);
    help_print_line(start_y, start_x, rows, line++, "  :w / :q / :wq   - Save / Quit / Save-Quit", false);

    /* Draw dismiss instruction at the bottom */
    const char *dismiss_msg = "[ Press Esc to close ]";
    int msg_len = (int)strlen(dismiss_msg);
    if (msg_len < width - 2 && start_y + height - 1 < state->view.screen_rows) {
        move(start_y + height - 1, start_x + (width - msg_len) / 2);
        attron(A_DIM);
        addstr(dismiss_msg);
        attroff(A_DIM);
    }

    attroff(COLOR_PAIR(4));
}
