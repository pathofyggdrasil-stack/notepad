
#include <stddef.h>
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <raylib.h>
#include "text_buffer.h"
#include "color_scheme.h"

int activeScheme = 0;
int totalSchemes = sizeof(colorSchemes) / sizeof(ColorScheme);

int main(void) {
    const int screenWidth = 1024;
    const int screenHeight = 768;

    InitWindow(screenWidth, screenHeight, "Modular C99 Notepad + Console");
    SetTargetFPS(60);

    // Initialize two separate text buffer instances using our module!
    TextBuffer *notepadBuffer = TB_Create(5000); // Multi-line workspace
    TextBuffer *consoleBuffer = TB_Create(1);    // Single-line command console

    int activeBuffer = 0; // 0 = Notepad, 1 = Console
    float blinkTimer = 0.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        blinkTimer += dt;

        // Switch focus between Notepad (Tab) and Console (` key or similar)
        if (IsKeyPressed(KEY_TAB)) {
            activeBuffer = 1 - activeBuffer;
            blinkTimer = 0.0f;
        }

        TextBuffer *active = (activeBuffer == 0) ? notepadBuffer : consoleBuffer;

        // --- Handle Text Input ---
        int key = GetCharPressed();
        while (key > 0) {
            if (key >= 32 && key <= 125) {
                TB_InsertChar(active, (char)key);
                blinkTimer = 0.0f;
            }
            key = GetCharPressed();
        }

        // Handle Keys
        if (IsKeyPressed(KEY_ENTER)) {
            if (activeBuffer == 1) {
                // Console execution action can happen here
                printf("Executed Command: %s\n", consoleBuffer->lines[0].chars);
                TB_Clear(consoleBuffer);
            } else {
                TB_InsertNewline(active);
            }
            blinkTimer = 0.0f;
        }
		// || IsKeyDown(KEY_BACKSPACE) may add this with a timer to not delete a line by mistake
        if (IsKeyPressed(KEY_BACKSPACE)) { TB_RemoveChar(active); blinkTimer = 0.0f; }
        if (IsKeyPressed(KEY_LEFT))      { TB_MoveCursorLeft(active); blinkTimer = 0.0f; }
        if (IsKeyPressed(KEY_RIGHT))     { TB_MoveCursorRight(active); blinkTimer = 0.0f; }
        if (IsKeyPressed(KEY_UP) && activeBuffer == 0)   { TB_MoveCursorUp(active); blinkTimer = 0.0f; }
        if (IsKeyPressed(KEY_DOWN) && activeBuffer == 0) { TB_MoveCursorDown(active); blinkTimer = 0.0f; }

        // --- Render ---
        BeginDrawing();
        ClearBackground(colorSchemes[activeScheme].background);

		// 1. Render Notepad Workspace (Top Section)
        DrawRectangle(0, 0, screenWidth, screenHeight - 50, (Color){30, 30, 30, 255});
        for (int i = 0; i < notepadBuffer->lineCount; i++) {
            float y = 20.0f + (i * 24.0f);
            DrawText(notepadBuffer->lines[i].chars, 20, (int)y, 20, (Color){220, 220, 220, 255});
        }
        
        // Notepad Cursor (Precise Pixel Measurement)
        if (activeBuffer == 0 && (int)(blinkTimer * 2.0f) % 2 == 0) {
            // Extract the exact substring from the current line up to the cursor column
            char sub[TB_MAX_LINE_LENGTH];
            int col = notepadBuffer->cursorCol;
            int lineIdx = notepadBuffer->cursorLine;
            
            memcpy(sub, notepadBuffer->lines[lineIdx].chars, col);
            sub[col] = '\0'; // Null-terminate the slice
            
            float textWidth = (float)MeasureText(sub, 20); // 20 is our font size
            float cx = 20.0f + textWidth;
            float cy = 20.0f + lineIdx * 24.0f;
            
            DrawRectangle((int)cx, (int)cy, 2, 22, WHITE);
        }

        // 2. Render Command Line Console (Bottom Section)
        DrawRectangle(0, screenHeight - 45, screenWidth, 45, (Color){15, 15, 15, 255});
        DrawRectangleLines(0, screenHeight - 45, screenWidth, 45, activeBuffer == 1 ? GREEN : DARKGRAY);
        DrawText(">", 10, screenHeight - 35, 20, GREEN);
        DrawText(consoleBuffer->lines[0].chars, 30, screenHeight - 35, 20, GREEN);

		// Console Cursor (Precise Pixel Measurement)
        if (activeBuffer == 1 && (int)(blinkTimer * 2.0f) % 2 == 0) {
            char sub[TB_MAX_LINE_LENGTH];
            int col = consoleBuffer->cursorCol;
            
            memcpy(sub, consoleBuffer->lines[0].chars, col);
            sub[col] = '\0';
            
            float textWidth = (float)MeasureText(sub, 20);
            float ccx = 30.0f + textWidth;
            
            DrawRectangle((int)ccx, screenHeight - 37, 2, 20, GREEN);
        }
        // Status bar info
        DrawText("[TAB] Switch Focus (Notepad / Console)", 10, screenHeight - 70, 14, GRAY);

        EndDrawing();
    }

    TB_Free(notepadBuffer);
    TB_Free(consoleBuffer);
    CloseWindow();
    return 0;
}
