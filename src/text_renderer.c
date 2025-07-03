#include "text_renderer.h"
#include "gruvbox.h"
#include "constants.h"
#include "raylib.h"

#include <stdio.h>
#include <string.h>

// Print a line number
void DrawLineNumber(int lineNumber, float* x, float y, Color color, Font font, float viewportWidth, float viewportHeight){
    char lineNumberString[16];
    snprintf(lineNumberString, sizeof(lineNumberString), "%d", lineNumber);
    EndScissorMode();
    DrawRectangle(0, y, NUMBER_LINE_WIDTH, FONT_SIZE+LINE_SPACING, Gruvbox[GRUVBOX_DARK2]);
    DrawTextEx(font, lineNumberString, (Vector2){0, y}, FONT_SIZE, 1, BLACK);  // Use DrawTextEx for custom font
    BeginScissorMode(NUMBER_LINE_WIDTH, TOOLBAR_HEIGHT, viewportWidth, viewportHeight);
    *x += NUMBER_LINE_WIDTH;
}

// Render text contiguously across multiple calls to the function
int DrawTextContiguous(const char* text, float* x, float* y, float startX, Color color, int lineNumber, Font font, bool wrapText, float maxWidth, float* maxX, float viewportWidth, float viewportHeight){
    if(text == NULL) return lineNumber;

    char buffer[1024];
    int bufferIndex = 0;

    if(lineNumber == 0){
        // Draw the first line number if it hasn't been drawn already
        DrawLineNumber(++lineNumber, x, *y, BLACK, font, viewportWidth, viewportHeight);  // Pass font
    }

    for(int i = 0; ; i++){

        if(text[i] == '\n' || text[i] == '\0'){
            buffer[bufferIndex] = '\0';
            
            // Check if we need to wrap text
            if(wrapText && maxWidth > 0){
                float textWidth = MeasureTextEx(font, buffer, FONT_SIZE, 1).x;
                if(*x + textWidth > maxWidth && bufferIndex > 0){
                    // Wrap to next line
                    *x = startX;
                    *y += FONT_SIZE + LINE_SPACING;
                    if(lineNumber != -1){
                        DrawLineNumber(lineNumber, x, *y, BLACK, font, viewportWidth, viewportHeight);
                    }
                }
            }
            
            DrawTextEx(font, buffer, (Vector2){*x, *y}, FONT_SIZE, 1, color);  // Replace DrawText with DrawTextEx
            float textWidth = MeasureTextEx(font, buffer, FONT_SIZE, 1).x;
            *x += textWidth;
            
            // Update maximum X position for horizontal scrolling
            if(maxX && *x > *maxX) *maxX = *x;

            if(text[i] == '\n'){
                *x = startX;
                *y += FONT_SIZE + LINE_SPACING; // Add some spacing between lines
                if(lineNumber != -1){
                    // Draw the next line number if line numbers are enabled
                    DrawLineNumber(++lineNumber, x, *y, BLACK, font, viewportWidth, viewportHeight);  // Pass font
                }
                bufferIndex = 0;
            }
            else{
                break;
            }
        }
        else{
            if(bufferIndex < sizeof(buffer) - 1){
                buffer[bufferIndex++] = text[i];
            }
        }
    }
    return lineNumber;
}

// Update DrawTextWithCyclingColors to accept Font and pass it to DrawTextContiguous
void DrawTextWithCyclingColors(const char* text, float* x, float* y, float startX, int* lineNumber, Font font, bool wrapText, float maxWidth, float* maxX, float viewportWidth, float viewportHeight){
    if(text == NULL){
        DrawTextContiguous("Failed to load text from file!", x, y, startX, WHITE, -1, font, wrapText, maxWidth, maxX, viewportWidth, viewportHeight);  // Pass font
        return;  // Early return for clarity
    }

    int colorIndex = 0;
    char word[512] = {'\0'};
    int wordLength = 0;

    for(int i = 0; text[i] != '\0'; i++){

        // Add character to word
        if(wordLength < sizeof(word) - 1){
            word[wordLength++] = text[i];
        }

        // Handle spaces, newlines, tabs, EOF as word delimiters
        if(text[i] == ' ' || text[i] == '\n' || text[i] == '\t' || text[i+1] == '\0'){
            // Draw the completed word with the current color
            word[wordLength] = '\0';
            Color color = Gruvbox[colorIndex++ % GRUVBOX_NUM_COLORS];
            *lineNumber = DrawTextContiguous(word, x, y, startX, color, *lineNumber, font, wrapText, maxWidth, maxX, viewportWidth, viewportHeight);
            wordLength = 0;
        }
    }
}
