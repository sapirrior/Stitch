#include <stdlib.h>
#include "stitch/types.h"
#include "buffer_internal.h"

static void buffer_free_undo_stack(UndoStack *stack) {
    UndoAction *curr = stack->head;
    while (curr) {
        UndoAction *next = curr->next;
        if (curr->text) free(curr->text);
        free(curr);
        curr = next;
    }
    stack->head = NULL;
    stack->current = NULL;
}

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
    buffer_free_undo_stack(&buf->undo_stack);
}
