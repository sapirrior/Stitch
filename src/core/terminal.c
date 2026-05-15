#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "stitch/core/terminal.h"
#include "stitch/ui/render.h"

static int raw_mode_active = 0;

void handleSigwinch(int sig) {
    (void)sig;
    E.resize_pending = 1;
}

void die(const char *s) {
    /* Leave alternate buffer on exit */
    write(STDOUT_FILENO, "\x1b[?1049l", 8);
    perror(s);
    exit(1);
}

void disableRawMode(void) {
    if (!raw_mode_active) return;
    /* Leave alternate buffer */
    write(STDOUT_FILENO, "\x1b[?1049l", 8);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
        die("tcsetattr");
    raw_mode_active = 0;
}

void enableRawMode(void) {
    if (raw_mode_active) return;
    if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("tcgetattr");
    
    static int atexit_registered = 0;
    if (!atexit_registered) {
        atexit(disableRawMode);
        atexit_registered = 1;
    }

    struct termios raw = E.orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
    raw_mode_active = 1;
    
    /* Enter alternate buffer */
    write(STDOUT_FILENO, "\x1b[?1049h", 8);

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handleSigwinch;
    sa.sa_flags = 0; /* Explicitly NOT setting SA_RESTART */
    sigaction(SIGWINCH, &sa, NULL);
}

int editorReadKey(void) {
    int nread;
    char c;
    nread = read(STDIN_FILENO, &c, 1);
    if (nread == 0) return KEY_NONE;
    if (nread == -1) {
        if (errno == EINTR) return KEY_RESIZE;
        if (errno == EAGAIN) return KEY_NONE;
        die("read");
    }

    if (c == '\x1b') {
        char seq[3];

        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
                if (seq[2] == '~') {
                    switch (seq[1]) {
                        case '1': return HOME_KEY;
                        case '3': return DEL_KEY;
                        case '4': return END_KEY;
                        case '5': return PAGE_UP;
                        case '6': return PAGE_DOWN;
                        case '7': return HOME_KEY;
                        case '8': return END_KEY;
                    }
                } else if (seq[2] >= '0' && seq[2] <= '9') {
                    /* Handle 2-digit sequences like \x1b[15~ */
                    char seq3;
                    if (read(STDIN_FILENO, &seq3, 1) != 1) return '\x1b';
                    if (seq3 == '~') {
                        int code = (seq[1] - '0') * 10 + (seq[2] - '0');
                        switch (code) {
                            case 15: /* F5 or some other key, just placeholder logic */ break;
                        }
                    }
                }
            } else {
                switch (seq[1]) {
                    case 'A': return ARROW_UP;
                    case 'B': return ARROW_DOWN;
                    case 'C': return ARROW_RIGHT;
                    case 'D': return ARROW_LEFT;
                    case 'H': return HOME_KEY;
                    case 'F': return END_KEY;
                }
            }
        } else if (seq[0] == 'O') {
            switch (seq[1]) {
                case 'H': return HOME_KEY;
                case 'F': return END_KEY;
            }
        }

        return '\x1b';
    } else {
        return c;
    }
}

int getWindowSize(int *rows, int *cols) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return -1;
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}
