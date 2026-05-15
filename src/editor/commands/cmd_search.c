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

    free(state->editor.search_query);
    state->editor.search_query = NULL;

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

    if (query[0] == '\0') return;
    state->editor.search_query = editorStrdup(query);

    int total_matches = 0;
    int current_match_idx = -1;

    for (size_t i = 0; i < state->buffer.num_lines; i++) {
        char *line_ptr = state->buffer.lines[i].render;
        while (line_ptr && (line_ptr = editorStrcasestr(line_ptr, query)) != NULL) {
            total_matches++;
            line_ptr += strlen(query);
        }
    }

    ssize_t current = last_match;
    if (current >= (ssize_t)state->buffer.num_lines) current = -1;

    for (size_t i = 0; i < state->buffer.num_lines; i++) {
        current += direction;
        if (current < 0) current = (ssize_t)state->buffer.num_lines - 1;
        else if (current >= (ssize_t)state->buffer.num_lines) current = 0;

        if (current < 0 || state->buffer.lines[current].render == NULL) continue;
        char *match = editorStrcasestr(state->buffer.lines[current].render, query);
        if (match) {
            last_match = current;
            state->view.cy = (int)current;
            state->view.cx = 0;
            if (state->buffer.lines[current].chars) {
                char *chars_match = editorStrcasestr(state->buffer.lines[current].chars, query);
                if (chars_match) state->view.cx = (int)(chars_match - state->buffer.lines[current].chars);
            }
            state->view.row_off = (int)state->buffer.num_lines;

            int found_count = 0;
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
        ui_set_status_message(state, "Match %d of %d (Arrows to navigate)", 
                               current_match_idx > 0 ? current_match_idx : 1, 
                               total_matches);
    } else {
        ui_set_status_message(state, "No matches found");
    }
}

void cmd_search_execute(StitchState *state) {
    int saved_cx = state->view.cx;
    int saved_cy = state->view.cy;
    int saved_col_off = state->view.col_off;
    int saved_row_off = state->view.row_off;

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
