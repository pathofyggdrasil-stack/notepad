#include "text_buffer.h"
#include <stdlib.h>
#include <string.h>

TextBuffer* TB_Create(int maxLines) {
    TextBuffer *tb = malloc(sizeof(TextBuffer));
    if (!tb) return NULL;
    
    tb->maxLines = maxLines;
    tb->lines = malloc(sizeof(TBLine) * maxLines);
    if (!tb->lines) {
        free(tb);
        return NULL;
    }
    
    TB_Clear(tb);
    return tb;
}

void TB_Free(TextBuffer *tb) {
    if (tb) {
        free(tb->lines);
        free(tb);
    }
}

void TB_Clear(TextBuffer *tb) {
    tb->lineCount = 1;
    tb->lines[0].length = 0;
    tb->lines[0].chars[0] = '\0';
    tb->cursorLine = 0;
    tb->cursorCol = 0;
}

void TB_InsertChar(TextBuffer *tb, char c) {
    TBLine *line = &tb->lines[tb->cursorLine];
    if (line->length >= TB_MAX_LINE_LENGTH - 1 || tb->cursorCol < 0) return;

    memmove(&line->chars[tb->cursorCol + 1], &line->chars[tb->cursorCol], (line->length - tb->cursorCol) * sizeof(char));
    line->chars[tb->cursorCol] = c;
    line->length++;
    line->chars[line->length] = '\0';
    tb->cursorCol++;
}

void TB_RemoveChar(TextBuffer *tb) {
    TBLine *line = &tb->lines[tb->cursorLine];

    if (tb->cursorCol > 0) {
        memmove(&line->chars[tb->cursorCol - 1], &line->chars[tb->cursorCol], (line->length - tb->cursorCol) * sizeof(char));
        tb->cursorCol--;
        line->length--;
        line->chars[line->length] = '\0';
    } else if (tb->cursorLine > 0) {
        // Merge with previous line
        TBLine *prevLine = &tb->lines[tb->cursorLine - 1];
        int prevLen = prevLine->length;

        if (prevLen + line->length < TB_MAX_LINE_LENGTH) {
            memcpy(&prevLine->chars[prevLen], line->chars, line->length);
            prevLine->length += line->length;
            prevLine->chars[prevLine->length] = '\0';

            // Shift remaining lines up
            memmove(&tb->lines[tb->cursorLine], &tb->lines[tb->cursorLine + 1], (tb->lineCount - tb->cursorLine - 1) * sizeof(TBLine));
            tb->lineCount--;
            tb->cursorLine--;
            tb->cursorCol = prevLen;
        }
    }
}

void TB_InsertNewline(TextBuffer *tb) {
    if (tb->lineCount >= tb->maxLines) return;

    // Shift lines down
    memmove(&tb->lines[tb->cursorLine + 2], &tb->lines[tb->cursorLine + 1], (tb->lineCount - tb->cursorLine - 1) * sizeof(TBLine));
    
    TBLine *current = &tb->lines[tb->cursorLine];
    TBLine *nextLine = &tb->lines[tb->cursorLine + 1];

    int remaining = current->length - tb->cursorCol;
    if (remaining > 0) {
        memcpy(nextLine->chars, &current->chars[tb->cursorCol], remaining);
        nextLine->length = remaining;
        nextLine->chars[remaining] = '\0';
        current->length = tb->cursorCol;
        current->chars[current->length] = '\0';
    } else {
        nextLine->length = 0;
        nextLine->chars[0] = '\0';
    }

    tb->lineCount++;
    tb->cursorLine++;
    tb->cursorCol = 0;
}

void TB_MoveCursorLeft(TextBuffer *tb) {
    if (tb->cursorCol > 0) {
        tb->cursorCol--;
    } else if (tb->cursorLine > 0) {
        tb->cursorLine--;
        tb->cursorCol = tb->lines[tb->cursorLine].length;
    }
}

void TB_MoveCursorRight(TextBuffer *tb) {
    TBLine *line = &tb->lines[tb->cursorLine];
    if (tb->cursorCol < line->length) {
        tb->cursorCol++;
    } else if (tb->cursorLine < tb->lineCount - 1) {
        tb->cursorLine++;
        tb->cursorCol = 0;
    }
}

void TB_MoveCursorUp(TextBuffer *tb) {
    if (tb->cursorLine > 0) {
        tb->cursorLine--;
        if (tb->cursorCol > tb->lines[tb->cursorLine].length) {
            tb->cursorCol = tb->lines[tb->cursorLine].length;
        }
    }
}

void TB_MoveCursorDown(TextBuffer *tb) {
    if (tb->cursorLine < tb->lineCount - 1) {
        tb->cursorLine++;
        if (tb->cursorCol > tb->lines[tb->cursorLine].length) {
            tb->cursorCol = tb->lines[tb->cursorLine].length;
        }
    }
}
