#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>
#include "stitch/core/terminal.h"

void core_die(const char *s) {
    if (!isendwin()) endwin();
    if (s) perror(s);
    exit(1);
}

void *editorMalloc(size_t size) {
    if (size == 0) return NULL;
    void *p = malloc(size);
    if (!p) core_die("malloc");
    return p;
}

void *editorRealloc(void *ptr, size_t size) {
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    void *p = realloc(ptr, size);
    if (!p) core_die("realloc");
    return p;
}

char *editorStrdup(const char *s) {
    if (!s) return NULL;
    char *p = strdup(s);
    if (!p) core_die("strdup");
    return p;
}

char *editorStrcasestr(const char *haystack, const char *needle) {
    if (!haystack || !needle) return NULL;
    if (!*needle) return (char *)haystack;
    
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
