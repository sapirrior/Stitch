#include <string.h>
#include <stdlib.h>
#include "stitch/types.h"
#include "stitch/core/terminal.h"
#include "stitch/ui/render.h"
#include "stitch/ui/prompt.h"
#include "../editor_internal.h"

static void editor_find_callback(StitchState *state, char *query, int key) {
    static ssize_t last_match = -1;
    static int direction = 1;

    if (key == '\r' || key == '\x1b') {
        last_match = -1;
        direction = 1;
        return;
    } else if (key == STITCH_ARROW_RIGHT || key == STITCH_ARROW_DOWN) {
        direction = 1;
    } else if (key == STITCH_ARROW_LEFT || key == STITCH_ARROW_UP) {
        direction = -1;
    } else {
        last_match = -1;
        direction = 1;
    }

    if (query[0] == '\0') {
        free(state->editor.search_query);
        state->editor.search_query = NULL;
        return;
    }
    
    char *new_query = editorStrdup(query);
    free(state->editor.search_query);
    state->editor.search_query = new_query;

    size_t total_matches = 0;
    size_t current_match_idx = 0;

    for (size_t i = 0; i < state->buffer.num_lines; i++) {
        char *line_ptr = state->buffer.lines[i].render;
        while (line_ptr && (line_ptr = editorStrcasestr(line_ptr, query)) != NULL) {
            total_matches++;
            line_ptr += strlen(query);
        }
    }

    ssize_t current = last_match;

    for (size_t i = 0; i < state->buffer.num_lines; i++) {
        current += direction;
        if (current < 0) current = (ssize_t)state->buffer.num_lines - 1;
        else if (current >= (ssize_t)state->buffer.num_lines) current = 0;

        if (current < 0) break;
        
        char *match = editorStrcasestr(state->buffer.lines[current].render, query);
        if (match) {
            last_match = current;
            state->view.cy = (size_t)current;
            state->view.cx = 0;
            if (state->buffer.lines[current].chars) {
                char *chars_match = editorStrcasestr(state->buffer.lines[current].chars, query);
                if (chars_match) state->view.cx = (size_t)(chars_match - state->buffer.lines[current].chars);
            }
            
            /* Center the match if it's outside the view */
            if (state->view.cy < state->view.row_off || state->view.cy >= state->view.row_off + state->view.screen_rows) {
                if (state->view.cy > (size_t)state->view.screen_rows / 2)
                    state->view.row_off = state->view.cy - state->view.screen_rows / 2;
                else
                    state->view.row_off = 0;
            }

            size_t found_count = 0;
            for (ssize_t j = 0; j <= current; j++) {
                char *lp = state->buffer.lines[j].render;
                while (lp && (lp = editorStrcasestr(lp, query)) != NULL) {
                    found_count++;
                    lp += strlen(query);
                }
            }
            current_match_idx = found_count;
            break;
        }
    }

    if (total_matches > 0) {
        ui_set_status_message(state, "Match %zu of %zu (Arrows to navigate)", 
                               current_match_idx > 0 ? current_match_idx : 1, 
                               total_matches);
    } else {
        ui_set_status_message(state, "No matches found");
    }
}

void cmd_search_execute(StitchState *state) {
    size_t saved_cx = state->view.cx;
    size_t saved_cy = state->view.cy;
    size_t saved_col_off = state->view.col_off;
    size_t saved_row_off = state->view.row_off;

    char *query = ui_prompt(state, "Search: %s (Use Arrows/Enter/Esc)",
                               editor_find_callback);

    if (query) {
        free(query);
    } else {
        state->view.cx = saved_cx;
        state->view.cy = saved_cy;
        state->view.col_off = saved_col_off;
        state->view.row_off = saved_row_off;
    }
}
