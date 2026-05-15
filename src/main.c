#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "stitch.h"
#include "stitch/editor/commands/parser.h"

/* The global EditorConfig instance */
EditorConfig E;

void initEditor(void) {
    E.cx = 0;
    E.cy = 0;
    E.row_off = 0;
    E.col_off = 0;
    E.num_lines = 0;
    E.lines = NULL;
    E.filename = NULL;
    E.status_msg[0] = '\0';
    E.dirty = 0;
    E.mode = MODE_NORMAL;
    E.last_key = 0;
    E.resize_pending = 0;
    E.shell_pid = -1;
    E.history_count = 0;
    for (int i = 0; i < 10; i++) E.history[i] = NULL;
    E.search_query = NULL;

    if (getWindowSize(&E.screen_rows, &E.screen_cols) == -1) {
        if (isatty(STDOUT_FILENO)) die("getWindowSize");
        /* Default for non-tty */
        E.screen_rows = 24;
        E.screen_cols = 80;
    }
    
    /* Ensure we have at least room for status/message bars and one line of text */
    if (E.screen_rows < 3) E.screen_rows = 3;
    if (E.screen_cols < 10) E.screen_cols = 10;

    /* Reserves space for status bar and command line */
    E.screen_rows -= 2; 

    setupSignals();
}

int main(int argc, char *argv[]) {
    enableRawMode();
    initEditor();

    if (argc >= 2) {
        if (editorOpen(argv[1]) == -1) {
            editorSetStatusMessage("Could not open file: %s", argv[1]);
        }
    }

    editorSetStatusMessage("HELP: :q = quit");

    while (1) {
        if (E.shell_pid != -1) editorUpdateShellStatus();
        editorRefreshScreen();
        editorProcessKeypress();
    }

    return 0;
}
