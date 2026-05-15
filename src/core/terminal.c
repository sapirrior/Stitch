#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ncurses.h>
#include "stitch/core/terminal.h"
#include "stitch/ui/render.h"

void die(const char *s) {
    endwin();
    perror(s);
    exit(1);
}

void disableRawMode(void) {
    endwin();
}

void enableRawMode(void) {
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    start_color();
    
    /* Initialize colors for "Organic Warmth" */
    if (can_change_color()) {
        /* Sage: 510, 588, 447 | Terra: 765, 478, 404 | Ochre: 843, 627, 294 */
        /* Earth: 184, 165, 157 | Cream: 968, 953, 910 */
        init_color(10, 510, 588, 447);
        init_color(11, 765, 478, 404);
        init_color(12, 843, 627, 294);
        init_color(13, 184, 165, 157);
        init_color(14, 968, 953, 910);
        
        init_pair(1, 13, 10); /* Earth on Sage (Normal Mode Block) */
        init_pair(2, 13, 11); /* Earth on Terra (Insert Mode Block) */
        init_pair(3, 13, 12); /* Earth on Ochre (Command Mode Block) */
        init_pair(4, 14, 0);  /* Cream on Black (Default Editor) */
        init_pair(5, 14, 13); /* Cream on Earth (Status Bar) */
    } else {
        /* 256-color Fallback for standard terminals (like Termux default) */
        /* 108: Sage-ish | 173: Terra-ish | 179: Ochre-ish | 235: Earth | 230: Cream */
        init_pair(1, 235, 108);
        init_pair(2, 235, 173);
        init_pair(3, 235, 179);
        init_pair(4, 230, 0);   /* Cream on Black */
        init_pair(5, 230, 235); /* Cream on Earth */
    }

    /* Set default background */
    bkgd(COLOR_PAIR(4));
}

int getWindowSize(int *rows, int *cols) {
    getmaxyx(stdscr, *rows, *cols);
    return 0;
}

void writeAll(int fd, const char *buf, size_t len) {
    /* Not strictly needed for ncurses, but keeping for compatibility if used elsewhere */
    (void)fd; (void)buf; (void)len;
}

int editorIsUtf8Start(unsigned char c) {
    return (c < 0x80) || (c >= 0xc0);
}

int editorRowByteToCol(const char *s, int len, int target_byte) {
    int col = 0;
    for (int i = 0; i < target_byte && i < len; i++) {
        if (editorIsUtf8Start((unsigned char)s[i])) col++;
    }
    return col;
}

int editorRowColToByte(const char *s, int len, int target_col) {
    int byte = 0;
    int col = 0;
    while (byte < len && col < target_col) {
        if (editorIsUtf8Start((unsigned char)s[byte])) col++;
        byte++;
    }
    while (byte < len && !editorIsUtf8Start((unsigned char)s[byte])) byte++;
    return byte;
}

int editorReadKey(void) {
    int c = getch();
    if (c == ERR) return STITCH_KEY_NONE;
    
    /* Normalize Enter keys */
    if (c == KEY_ENTER || c == '\n' || c == '\r') return '\r';

    /* Normalize Backspace keys */
    if (c == 127 || c == KEY_BACKSPACE) return STITCH_BACKSPACE;

    /* Handle Ctrl Keys */
    if (c < 32) return c;
    
    /* Map other ncurses keys to our enum */
    switch (c) {
        case KEY_LEFT: return STITCH_ARROW_LEFT;
        case KEY_RIGHT: return STITCH_ARROW_RIGHT;
        case KEY_UP: return STITCH_ARROW_UP;
        case KEY_DOWN: return STITCH_ARROW_DOWN;
        case KEY_DC: return STITCH_DEL_KEY;
        case KEY_HOME: return STITCH_HOME_KEY;
        case KEY_END: return STITCH_END_KEY;
        case KEY_PPAGE: return STITCH_PAGE_UP;
        case KEY_NPAGE: return STITCH_PAGE_DOWN;
        case KEY_RESIZE: return STITCH_KEY_RESIZE;
    }

    return c;
}

void *editorMalloc(size_t size) {
    void *p = malloc(size);
    if (!p) die("malloc");
    return p;
}

void *editorRealloc(void *ptr, size_t size) {
    void *p = realloc(ptr, size);
    if (!p) die("realloc");
    return p;
}

char *editorStrdup(const char *s) {
    char *p = strdup(s);
    if (!p) die("strdup");
    return p;
}

char *editorStrcasestr(const char *haystack, const char *needle) {
    if (!haystack || !needle || !*needle) return (char *)haystack;
    for (; *haystack; haystack++) {
        if (toupper((unsigned char)*haystack) == toupper((unsigned char)*needle)) {
            const char *h, *n;
            for (h = haystack, n = needle; *h && *n; h++, n++) {
                if (toupper((unsigned char)*h) != toupper((unsigned char)*n)) break;
            }
            if (!*n) return (char *)haystack;
        }
    }
    return NULL;
}
