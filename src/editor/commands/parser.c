#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include "stitch/editor/commands/parser.h"
#include "stitch/core/terminal.h"
#include "stitch/buffer/io.h"
#include "stitch/buffer/engine.h"
#include "stitch/ui/prompt.h"

static void editorExecuteShellBackground(const char *cmd) {
    pid_t pid = fork();
    if (pid == -1) {
        editorSetStatusMessage("Fork failed: %s", strerror(errno));
        return;
    }

    if (pid == 0) {
        /* Child: Run command silently */
        int devnull = open("/dev/null", O_RDWR);
        if (devnull != -1) {
            dup2(devnull, STDIN_FILENO);
            dup2(devnull, STDOUT_FILENO);
            dup2(devnull, STDERR_FILENO);
            close(devnull);
        }
        
        char *args[] = {"sh", "-c", (char *)cmd, NULL};
        execvp("sh", args);
        exit(1);
    } else {
        /* Parent */
        E.shell_pid = pid;
        editorSetStatusMessage("Running: %s...", cmd);
    }
}

void editorUpdateShellStatus(void) {
    if (E.shell_pid == -1) return;

    int status;
    pid_t result = waitpid(E.shell_pid, &status, WNOHANG);
    
    if (result > 0) {
        /* Process finished */
        if (WIFEXITED(status)) {
            editorSetStatusMessage("Process exited with code %d", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            editorSetStatusMessage("Process killed by signal %d", WTERMSIG(status));
        } else {
            editorSetStatusMessage("Process finished");
        }
        E.shell_pid = -1;
    } else if (result == -1) {
        /* Error in waitpid */
        if (errno != ECHILD) {
            editorSetStatusMessage("Waitpid error: %s", strerror(errno));
        }
        E.shell_pid = -1;
    }
}

void handleCommand(const char *cmd) {
    if (cmd[0] == '!') {
        if (cmd[1] == '\0') {
            editorSetStatusMessage("Usage: !<command>");
            return;
        }
        editorExecuteShellBackground(cmd + 1);
        return;
    }

    if (strcmp(cmd, "q") == 0) {
        if (E.dirty) {
            editorSetStatusMessage("No write since last change (add ! to override)");
            return;
        }
        exit(0);
    } else if (strcmp(cmd, "q!") == 0) {
        exit(0);
    } else if (strcmp(cmd, "w") == 0) {
        editorSave();
    } else if (strcmp(cmd, "wq") == 0) {
        editorSave();
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
    } else {
        editorSetStatusMessage("Unknown command: %s", cmd);
    }
}
