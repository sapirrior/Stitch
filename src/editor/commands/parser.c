#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "stitch/editor/commands/parser.h"
#include "stitch/buffer/io.h"
#include "stitch/buffer/engine.h"
#include "stitch/ui/prompt.h"

void handleCommand(const char *cmd) {
    if (strcmp(cmd, "q") == 0) {
        if (E.dirty) {
            editorSetStatusMessage("No write since last change (add ! to override)");
            return;
        }
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);
        exit(0);
    } else if (strcmp(cmd, "q!") == 0) {
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);
        exit(0);
    } else if (strcmp(cmd, "w") == 0) {
        editorSave();
    } else if (strcmp(cmd, "wq") == 0) {
        editorSave();
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);
        exit(0);
    } else if (strncmp(cmd, "e ", 2) == 0) {
        if (E.dirty) {
            editorSetStatusMessage("No write since last change (add ! to override)");
            return;
        }
        char *filename = strdup(cmd + 2);
        if (editorOpen(filename) == -1) {
            editorSetStatusMessage("Could not open file: %s", filename);
        }
        free(filename);
    }
 else {
        editorSetStatusMessage("Unknown command: %s", cmd);
    }
}
