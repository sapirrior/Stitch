#include <ncurses.h>
#include <string.h>
#include <stdio.h>
#include "stitch/types.h"
#include "stitch/core/terminal.h"
#include "ui_internal.h"

static int get_rx_from_cx(Line *line, size_t target_cx) {
    int rx = 0;
    for (size_t j = 0; j < target_cx && j < line->size; j++) {
        unsigned char c = (unsigned char)line->chars[j];
        if (c == '\t') {
            rx += (STITCH_TAB_STOP - (rx % STITCH_TAB_STOP));
        } else if (c < 32 || c == 127) {
            rx += 2;
        } else if (editorIsUtf8Start(c)) {
            rx++;
        }
    }
    return rx;
}

static bool find_matching_bracket(StitchState *state, size_t *match_y, size_t *match_x) {
    if (state->view.cy >= state->buffer.num_lines) return false;
    Line *line = &state->buffer.lines[state->view.cy];
    if (state->view.cx >= line->size) return false;

    char c = line->chars[state->view.cx];
    char pair;
    int dir = 0;
    
    if (c == '(') { pair = ')'; dir = 1; }
    else if (c == '[') { pair = ']'; dir = 1; }
    else if (c == '{') { pair = '}'; dir = 1; }
    else if (c == ')') { pair = '('; dir = -1; }
    else if (c == ']') { pair = '['; dir = -1; }
    else if (c == '}') { pair = '{'; dir = -1; }
    else return false;

    int depth = 1;
    size_t y = state->view.cy;
    size_t x = state->view.cx;

    while (true) {
        if (dir == 1) {
            x++;
            if (x >= state->buffer.lines[y].size) {
                y++;
                if (y >= state->buffer.num_lines) return false;
                x = 0;
                continue;
            }
        } else {
            if (x == 0) {
                if (y == 0) return false;
                y--;
                x = state->buffer.lines[y].size;
                if (x > 0) x--;
                else continue;
            } else {
                x--;
            }
        }
        
        char cur = state->buffer.lines[y].chars[x];
        if (cur == c) depth++;
        else if (cur == pair) depth--;

        if (depth == 0) {
            *match_y = y;
            *match_x = x;
            return true;
        }
    }
}

void ui_text_grid_draw(StitchState *state) {
    int gutter_width = ui_get_gutter_width(state);
    
    size_t match_y = 0, match_x = 0;
    bool has_match = find_matching_bracket(state, &match_y, &match_x);

    attron(COLOR_PAIR(4)); /* Cream on Black */
    for (int y = 0; y < state->view.screen_rows; y++) {
        size_t filerow = (size_t)y + state->view.row_off;
        move(y, 0);
        clrtoeol();

        if (state->ui.show_line_numbers) {
            attron(COLOR_PAIR(5)); /* Cream on Earth */
            if (filerow < state->buffer.num_lines) {
                mvprintw(y, 0, " %*zu", gutter_width - 2, filerow + 1);
            } else {
                for (int i = 0; i < gutter_width - 1; i++) mvaddch(y, i, ' ');
            }
            attroff(COLOR_PAIR(5));
            attron(COLOR_PAIR(4));
            mvaddch(y, gutter_width - 1, ' '); /* Gap in default color */
        }

        if (filerow >= state->buffer.num_lines) {
            if (state->buffer.num_lines == 0 && y == state->view.screen_rows / 3) {
                char welcome[80];
                int welcomelen = snprintf(welcome, sizeof(welcome),
                    "Stitch editor -- version %s", STITCH_VERSION);
                if (welcomelen > state->view.screen_cols - gutter_width) welcomelen = state->view.screen_cols - gutter_width;
                int x = gutter_width + (state->view.screen_cols - gutter_width - welcomelen) / 2;
                mvaddnstr(y, x, welcome, welcomelen);
            }
        } else {
            int rlen = state->buffer.lines[filerow].rsize;
            char *rline = state->buffer.lines[filerow].render;

            int coff = editorRowColToByte(rline, rlen, state->view.col_off);
            int draw_len = editorRowColToByte(&rline[coff], rlen - coff, state->view.screen_cols - gutter_width);

            int hl1_byte = -1;
            int hl2_byte = -1;
            if (has_match) {
                if (filerow == state->view.cy) {
                    int rx = get_rx_from_cx(&state->buffer.lines[filerow], state->view.cx);
                    if (rx >= (int)state->view.col_off && rx < (int)state->view.col_off + (state->view.screen_cols - gutter_width)) {
                        hl1_byte = editorRowColToByte(rline, rlen, rx) - coff;
                    }
                }
                if (filerow == match_y) {
                    int rx = get_rx_from_cx(&state->buffer.lines[filerow], match_x);
                    if (rx >= (int)state->view.col_off && rx < (int)state->view.col_off + (state->view.screen_cols - gutter_width)) {
                        hl2_byte = editorRowColToByte(rline, rlen, rx) - coff;
                    }
                }
            }

            move(y, gutter_width);
            if (state->editor.search_query && state->editor.search_query[0] != '\0' && draw_len > 0) {
                char *draw_ptr = &rline[coff];
                int current_idx = 0;
                int query_len = (int)strlen(state->editor.search_query);

                while (current_idx < draw_len) {
                    char *match = editorStrcasestr(&draw_ptr[current_idx], state->editor.search_query);
                    if (match && (match - draw_ptr) < draw_len) {
                        int match_idx = (int)(match - draw_ptr);
                        for (int i = current_idx; i < match_idx; i++) {
                            if (i == hl1_byte || i == hl2_byte) attron(COLOR_PAIR(2));
                            addch(draw_ptr[i]);
                            if (i == hl1_byte || i == hl2_byte) { attroff(COLOR_PAIR(2)); attron(COLOR_PAIR(4)); }
                        }
                        
                        int qlen = query_len;
                        if (match_idx + qlen > draw_len) qlen = draw_len - match_idx;
                        
                        attron(COLOR_PAIR(3)); /* Ochre (Highlight) */
                        for (int i = 0; i < qlen; i++) {
                            int b = match_idx + i;
                            if (b == hl1_byte || b == hl2_byte) { attroff(COLOR_PAIR(3)); attron(COLOR_PAIR(2)); }
                            addch(draw_ptr[b]);
                            if (b == hl1_byte || b == hl2_byte) { attroff(COLOR_PAIR(2)); attron(COLOR_PAIR(3)); }
                        }
                        attroff(COLOR_PAIR(3));
                        attron(COLOR_PAIR(4));
                        
                        current_idx = match_idx + qlen;
                        if (qlen == 0) break;
                    } else {
                        for (int i = current_idx; i < draw_len; i++) {
                            if (i == hl1_byte || i == hl2_byte) attron(COLOR_PAIR(2)); /* Terra for brackets */
                            addch(draw_ptr[i]);
                            if (i == hl1_byte || i == hl2_byte) { attroff(COLOR_PAIR(2)); attron(COLOR_PAIR(4)); }
                        }
                        break;
                    }
                }
            } else {
                for (int i = 0; i < draw_len; i++) {
                    if (i == hl1_byte || i == hl2_byte) attron(COLOR_PAIR(2)); /* Terra for brackets */
                    addch(rline[coff + i]);
                    if (i == hl1_byte || i == hl2_byte) { attroff(COLOR_PAIR(2)); attron(COLOR_PAIR(4)); }
                }
            }
        }
    }
    attroff(COLOR_PAIR(4));
}
