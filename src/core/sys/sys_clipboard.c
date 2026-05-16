#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stitch/core/terminal.h"

// Internal fallback
static char *internal_clipboard = NULL;
static size_t internal_clipboard_len = 0;

static const char *cached_set_cmd = (const char *)-1;
static const char *get_clipboard_set_cmd(void) {
    if (cached_set_cmd != (const char *)-1) return cached_set_cmd;
    if (system("which termux-clipboard-set >/dev/null 2>&1") == 0) return cached_set_cmd = "termux-clipboard-set";
    if (system("which pbcopy >/dev/null 2>&1") == 0) return cached_set_cmd = "pbcopy";
    if (system("which xclip >/dev/null 2>&1") == 0) return cached_set_cmd = "xclip -selection clipboard";
    if (system("which xsel >/dev/null 2>&1") == 0) return cached_set_cmd = "xsel --clipboard --input";
    return cached_set_cmd = NULL;
}

static const char *cached_get_cmd = (const char *)-1;
static const char *get_clipboard_get_cmd(void) {
    if (cached_get_cmd != (const char *)-1) return cached_get_cmd;
    if (system("which termux-clipboard-get >/dev/null 2>&1") == 0) return cached_get_cmd = "termux-clipboard-get";
    if (system("which pbpaste >/dev/null 2>&1") == 0) return cached_get_cmd = "pbpaste";
    if (system("which xclip >/dev/null 2>&1") == 0) return cached_get_cmd = "xclip -selection clipboard -o";
    if (system("which xsel >/dev/null 2>&1") == 0) return cached_get_cmd = "xsel --clipboard --output";
    return cached_get_cmd = NULL;
}

void sys_clipboard_set(const char *text, size_t len) {
    if (!text || len == 0) return;
    
    // Always set internal clipboard as fallback
    free(internal_clipboard);
    internal_clipboard = editorMalloc(len + 1);
    memcpy(internal_clipboard, text, len);
    internal_clipboard[len] = '\0';
    internal_clipboard_len = len;

    const char *cmd = get_clipboard_set_cmd();
    if (cmd) {
        FILE *fp = popen(cmd, "w");
        if (fp) {
            fwrite(text, 1, len, fp);
            pclose(fp);
        }
    }
}

char *sys_clipboard_get(size_t *len) {
    const char *cmd = get_clipboard_get_cmd();
    if (cmd) {
        FILE *fp = popen(cmd, "r");
        if (fp) {
            size_t cap = 1024;
            size_t size = 0;
            char *buf = editorMalloc(cap);
            size_t bytes_read;
            while ((bytes_read = fread(buf + size, 1, cap - size, fp)) > 0) {
                size += bytes_read;
                if (size >= cap) {
                    cap *= 2;
                    buf = editorRealloc(buf, cap);
                }
            }
            pclose(fp);
            
            if (size > 0) {
                *len = size;
                buf[size] = '\0';
                return buf;
            }
            free(buf);
        }
    }
    
    // Fallback to internal
    if (internal_clipboard && internal_clipboard_len > 0) {
        *len = internal_clipboard_len;
        char *dup = editorMalloc(internal_clipboard_len + 1);
        memcpy(dup, internal_clipboard, internal_clipboard_len);
        dup[internal_clipboard_len] = '\0';
        return dup;
    }
    
    *len = 0;
    return NULL;
}
