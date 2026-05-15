#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include "stitch/types.h"
#include "stitch/core/terminal.h"
#include "ui_internal.h"

void ui_text_grid_draw(StitchState *state) {
    attron(COLOR_PAIR(4)); /* Cream on Earth */
    for (int y = 0; y < state->view.screen_rows; y++) {
        int filerow = y + state->view.row_off;
        move(y, 0);
        clrtoeol();

        if (filerow >= state->buffer.num_lines) {
            if (state->buffer.num_lines == 0 && y == state->view.screen_rows / 3) {
                char welcome[80];
                int welcomelen = snprintf(welcome, sizeof(welcome),
                    "Stitch editor -- version %s", STITCH_VERSION);
                if (welcomelen > state->view.screen_cols) welcomelen = state->view.screen_cols;
                int x = (state->view.screen_cols - welcomelen) / 2;
                mvaddnstr(y, x, welcome, welcomelen);
            }
        } else {
            int rlen = state->buffer.lines[filerow].rsize;
            char *rline = state->buffer.lines[filerow].render;

            int coff = editorRowColToByte(rline, rlen, state->view.col_off);
            int draw_len = editorRowColToByte(&rline[coff], rlen - coff, state->view.screen_cols);

            if (state->editor.search_query && state->editor.search_query[0] != '\0' && draw_len > 0) {
                char *draw_ptr = &rline[coff];
                int current_idx = 0;
                int query_len = (int)strlen(state->editor.search_query);

                while (current_idx < draw_len) {
                    char *match = editorStrcasestr(&draw_ptr[current_idx], state->editor.search_query);
                    if (match && (match - draw_ptr) < draw_len) {
                        int match_idx = (int)(match - draw_ptr);
                        addnstr(&draw_ptr[current_idx], match_idx - current_idx);
                        
                        int qlen = query_len;
                        if (match_idx + qlen > draw_len) qlen = draw_len - match_idx;
                        
                        attron(COLOR_PAIR(3)); /* Ochre (Highlight) */
                        addnstr(match, qlen);
                        attroff(COLOR_PAIR(3));
                        attron(COLOR_PAIR(4));
                        
                        current_idx = match_idx + qlen;
                        if (qlen == 0) break; /* Avoid infinite loop */
                    } else {
                        addnstr(&draw_ptr[current_idx], draw_len - current_idx);
                        break;
                    }
                }
            } else {
                addnstr(&rline[coff], draw_len);
            }
        }
    }
    attroff(COLOR_PAIR(4));
}
