#ifndef TEXT_BUFFER_H
#define TEXT_BUFFER_H

#define TB_MAX_LINE_LENGTH 1024

typedef struct {
    char chars[TB_MAX_LINE_LENGTH];
    int length;
} TBLine;

typedef struct {
    TBLine *lines;
    int lineCount;
    int maxLines;
    int cursorLine;
    int cursorCol;
} TextBuffer;

// Lifecycle
TextBuffer* TB_Create(int maxLines);
void TB_Free(TextBuffer *tb);
void TB_Clear(TextBuffer *tb);

// Editing Operations
void TB_InsertChar(TextBuffer *tb, char c);
void TB_RemoveChar(TextBuffer *tb);
void TB_InsertNewline(TextBuffer *tb);

// Navigation
void TB_MoveCursorLeft(TextBuffer *tb);
void TB_MoveCursorRight(TextBuffer *tb);
void TB_MoveCursorUp(TextBuffer *tb);
void TB_MoveCursorDown(TextBuffer *tb);
void TB_SetCursor(TextBuffer *tb, int line, int col);

#endif
