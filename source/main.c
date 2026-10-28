
#include <stddef.h>
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <raylib.h>

#define MAX_LINE_LENGTH 1024
#define MAX_LINES 10000

// Struct to store character styling/token metadata (Ready for future Syntax Highlighting)
typedef struct {
    char ch;
    Color color; // Reserved for syntax highlighting token color
} TextChar;

typedef struct {
    TextChar chars[MAX_LINE_LENGTH];
    int length;
} Line;

// Color Scheme Structure
typedef struct {
    const char *name;
    Color background;
    Color text;
    Color cursor;
    Color lineNumbersBg;
    Color lineNumbersText;
    Color currentLineHighlight;
} ColorScheme;

// Available Color Schemes
ColorScheme colorSchemes[] = {
    {
        .name = "Dark (Default)",
        .background = (Color){24, 24, 24, 255},
        .text = (Color){220, 220, 220, 255},
        .cursor = (Color){255, 255, 255, 255},
        .lineNumbersBg = (Color){32, 32, 32, 255},
        .lineNumbersText = (Color){100, 100, 100, 255},
        .currentLineHighlight = (Color){40, 40, 40, 255}
    },
    {
        .name = "Light",
        .background = (Color){250, 250, 250, 255},
        .text = (Color){30, 30, 30, 255},
        .cursor = (Color){0, 0, 0, 255},
        .lineNumbersBg = (Color){230, 230, 230, 255},
        .lineNumbersText = (Color){140, 140, 140, 255},
        .currentLineHighlight = (Color){240, 240, 245, 255}
    },
    {
        .name = "Monokai",
        .background = (Color){39, 40, 34, 255},
        .text = (Color){248, 248, 242, 255},
        .cursor = (Color){248, 248, 240, 255},
        .lineNumbersBg = (Color){49, 50, 44, 255},
        .lineNumbersText = (Color){144, 144, 138, 255},
        .currentLineHighlight = (Color){62, 61, 50, 255}
    },
    {
        .name = "Matrix",
        .background = (Color){5, 15, 5, 255},
        .text = (Color){0, 255, 65, 255},
        .cursor = (Color){0, 255, 65, 255},
        .lineNumbersBg = (Color){10, 25, 10, 255},
        .lineNumbersText = (Color){0, 120, 30, 255},
        .currentLineHighlight = (Color){15, 35, 15, 255}
    }
};

int activeScheme = 0;
int totalSchemes = sizeof(colorSchemes) / sizeof(ColorScheme);

// Helper function to insert a character into a line
void LineInsertChar(Line *line, int col, char c, Color defaultColor) {
    if (line->length >= MAX_LINE_LENGTH - 1 || col < 0 || col > line->length) return;
    memmove(&line->chars[col + 1], &line->chars[col], (line->length - col) * sizeof(TextChar));
    line->chars[col].ch = c;
    line->chars[col].color = defaultColor;
    line->length++;
}

// Helper function to remove a character from a line
void LineRemoveChar(Line *line, int col) {
    if (col < 0 || col >= line->length) return;
    memmove(&line->chars[col], &line->chars[col + 1], (line->length - col - 1) * sizeof(TextChar));
    line->length--;
}

int	main(void) {
    const int screenWidth = 1024;
    const int screenHeight = 768;

    // Document State
	int maxLines = MAX_LINES;
    Line *lines = malloc(sizeof(Line) * maxLines);
    if (lines == NULL) {
        fprintf(stderr, "Failed to allocate memory for document.\n");
        CloseWindow();
        return 1;
    }
    
    int lineCount = 1;
    lines[0].length = 0;

    // Cursor position in line/col space
    int cursorLine = 0;
    int cursorCol = 0;

    // Viewport scrolling
    int scrollLineOffset = 0;

    // Typography
    const int fontSize = 20;
    // Font font = GetFontDefault();
    // const float charWidth = (float)font.baseSize; // Using default fixed-width font spacing
    const float lineHeight = 28.0f;
    const float marginX = 70.0f;
    const float marginY = 10.0f;

    float cursorBlinkTimer = 0.0f;

    InitWindow(screenWidth, screenHeight, "emipad");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        cursorBlinkTimer += deltaTime;

        // --- Theme Switching ---
        if (IsKeyPressed(KEY_F1)) {
            activeScheme = (activeScheme + 1) % totalSchemes;
        }

        ColorScheme theme = colorSchemes[activeScheme];

        // --- Text Input Handling ---
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key <= 125)) { // Printable ASCII range
                LineInsertChar(&lines[cursorLine], cursorCol, (char)key, theme.text);
                cursorCol++;
                cursorBlinkTimer = 0.0f;
            }
            key = GetCharPressed();
        }

        // --- Keyboard Controls ---
        if (IsKeyPressed(KEY_ENTER)) {
            if (lineCount < MAX_LINES) {
                // Split line at cursor position
                memmove(&lines[cursorLine + 2], &lines[cursorLine + 1], (lineCount - cursorLine - 1) * sizeof(Line));
                lines[cursorLine + 1].length = 0;

                int remaining = lines[cursorLine].length - cursorCol;
                if (remaining > 0) {
                    memcpy(lines[cursorLine + 1].chars, &lines[cursorLine].chars[cursorCol], remaining * sizeof(TextChar));
                    lines[cursorLine + 1].length = remaining;
                    lines[cursorLine].length = cursorCol;
                }

                lineCount++;
                cursorLine++;
                cursorCol = 0;
                cursorBlinkTimer = 0.0f;
            }
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (cursorCol > 0) {
                LineRemoveChar(&lines[cursorLine], cursorCol - 1);
                cursorCol--;
            } else if (cursorLine > 0) {
                // Merge current line into previous line
                int prevLen = lines[cursorLine - 1].length;
                int currentLen = lines[cursorLine].length;

                if (prevLen + currentLen < MAX_LINE_LENGTH) {
                    memcpy(&lines[cursorLine - 1].chars[prevLen], lines[cursorLine].chars, currentLen * sizeof(TextChar));
                    lines[cursorLine - 1].length += currentLen;

                    memmove(&lines[cursorLine], &lines[cursorLine + 1], (lineCount - cursorLine - 1) * sizeof(Line));
                    lineCount--;
                    cursorLine--;
                    cursorCol = prevLen;
                }
            }
            cursorBlinkTimer = 0.0f;
        }

        // Arrow Key Navigation
        if (IsKeyPressed(KEY_LEFT) && (cursorCol > 0 || cursorLine > 0)) {
            if (cursorCol > 0) cursorCol--;
            else {
                cursorLine--;
                cursorCol = lines[cursorLine].length;
            }
            cursorBlinkTimer = 0.0f;
        }

        if (IsKeyPressed(KEY_RIGHT)) {
            if (cursorCol < lines[cursorLine].length) cursorCol++;
            else if (cursorLine < lineCount - 1) {
                cursorLine++;
                cursorCol = 0;
            }
            cursorBlinkTimer = 0.0f;
        }

        if (IsKeyPressed(KEY_UP) && cursorLine > 0) {
            cursorLine--;
            if (cursorCol > lines[cursorLine].length) cursorCol = lines[cursorLine].length;
            cursorBlinkTimer = 0.0f;
        }

        if (IsKeyPressed(KEY_DOWN) && cursorLine < lineCount - 1) {
            cursorLine++;
            if (cursorCol > lines[cursorLine].length) cursorCol = lines[cursorLine].length;
            cursorBlinkTimer = 0.0f;
        }

        // Scrolling
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            scrollLineOffset -= (int)(wheel * 3);
            if (scrollLineOffset < 0) scrollLineOffset = 0;
        }

        // Clamp cursor position into visible area
        int visibleLines = (GetScreenHeight() - marginY * 2) / lineHeight;
        if (cursorLine < scrollLineOffset) scrollLineOffset = cursorLine;
        if (cursorLine >= scrollLineOffset + visibleLines) scrollLineOffset = cursorLine - visibleLines + 1;

        // --- Render ---
        BeginDrawing();
        ClearBackground(theme.background);

        // 1. Highlight Current Line
        float currentLineY = marginY + (cursorLine - scrollLineOffset) * lineHeight;
        DrawRectangle(0, (int)currentLineY, GetScreenWidth(), (int)lineHeight, theme.currentLineHighlight);

        // 2. Render Text Content
        for (int i = scrollLineOffset; i < lineCount && i < scrollLineOffset + visibleLines + 1; i++) {
            float lineY = marginY + (i - scrollLineOffset) * lineHeight;

            // Render characters per line
            for (int j = 0; j < lines[i].length; j++) {
                char chStr[2] = { lines[i].chars[j].ch, '\0' };
                // Using individual character colors allows syntax highlighting implementation
                Color charColor = lines[i].chars[j].color;
                
                // Fallback if token color is clear/default
                if (charColor.a == 0) charColor = theme.text; 

                DrawText(chStr, (int)(marginX + j * (fontSize * 0.6f)), (int)lineY, fontSize, charColor);
            }
        }

        // 3. Render Cursor
        if ((int)(cursorBlinkTimer * 2.0f) % 2 == 0) {
            float cursorX = marginX + cursorCol * (fontSize * 0.6f);
            DrawRectangle((int)cursorX, (int)currentLineY, 2, (int)lineHeight, theme.cursor);
        }

        // 4. Render Sidebar / Line Numbers
        DrawRectangle(0, 0, (int)marginX - 10, GetScreenHeight(), theme.lineNumbersBg);
        for (int i = scrollLineOffset; i < lineCount && i < scrollLineOffset + visibleLines + 1; i++) {
            float lineY = marginY + (i - scrollLineOffset) * lineHeight;
            char numBuf[16];
            snprintf(numBuf, sizeof(numBuf), "%4d", i + 1);
            DrawText(numBuf, 10, (int)lineY, fontSize - 2, theme.lineNumbersText);
        }

        // 5. Render Status Bar
        char statusBuffer[128];
        snprintf(statusBuffer, sizeof(statusBuffer), "[F1] Theme: %s | Line: %d, Col: %d", theme.name, cursorLine + 1, cursorCol + 1);
        DrawRectangle(0, GetScreenHeight() - 25, GetScreenWidth(), 25, theme.lineNumbersBg);
        DrawText(statusBuffer, 10, GetScreenHeight() - 20, 16, theme.text);

        EndDrawing();
    }
    CloseWindow();
	free(lines);
    return 0;
}
