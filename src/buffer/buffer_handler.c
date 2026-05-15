#include <stdlib.h>
#include "stitch/types.h"
#include "buffer_internal.h"

void buffer_free(StitchBuffer *buf) {
    if (buf->lines) {
        for (size_t i = 0; i < buf->num_lines; i++) {
            free(buf->lines[i].chars);
            free(buf->lines[i].render);
        }
        free(buf->lines);
        buf->lines = NULL;
    }
    buf->num_lines = 0;
}
