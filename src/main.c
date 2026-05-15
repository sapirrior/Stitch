#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "stitch.h"

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

    if (getWindowSize(&E.screen_rows, &E.screen_cols) == -1) die("getWindowSize");
    /* Reserves space for status bar and command line */
    E.screen_rows -= 2; 
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
        editorRefreshScreen();
        editorProcessKeypress();
    }

    return 0;
}
